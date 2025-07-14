#include "device_driver.h"
#include <stdlib.h>

#define DIPLAY_MODE 3
#define LCDW                (320)
#define LCDH                (240)
#define X_MIN               (10)
#define X_MAX               (LCDW - 10)
#define Y_MIN               (20)
#define Y_MAX               (LCDH - 1)

#define TIMER_PERIOD        (10)
#define RIGHT               (1)
#define LEFT                (-1)

#define BASE                (500)

#define MONSTER_STEP        (5)
#define MONSTER_SIZE_X      (16)
#define MONSTER_SIZE_Y      (16)
#define MONSTER_COLOR       (1)

#define UI_STEP             (8)
#define UI_SIZE_X           (40)
#define UI_SIZE_Y           (16)
#define UI_COLOR            (2)

#define MY_STEP             (8)
#define MY_SIZE_X           (8)
#define MY_SIZE_Y           (8)
#define MY_COLOR            (2)
#define MY_CONFUSE_COLOR    (3)

#define GUN_STEP            (2)
#define GUN_SIZE_X          (2)
#define GUN_SIZE_Y          (2)
#define GUN_COLOR           (1)

#define BOSS_SIZE_X         (25)
#define BOSS_SIZE_Y         (25)
#define BOSS_COLOR1          (4)
#define BOSS_COLOR2          (0)
#define BOSS_COLOR3          (2)
#define BOSS_STEP1           (5)
#define BOSS_STEP2           (8)
#define BOSS_STEP3           (11)

#define BOSS_HP_UI_X        (280)
#define BOSS_HP_UI_Y        (16)

#define BOSS_LOSE_UI_X      (1)
#define BOSS_LOSE_UI_Y      (16)  

#define HP_SIZE             (999)

#define PHASE2              (200)
#define PHASE3              (100)

#define BANANA_SIZE_X       (14)
#define BANANA_SIZE_Y       (14)
#define BANANA_COLOR        (1)
#define BANANA_STEP         (6)

#define BACK_COLOR          (5)
#define GAME_OVER           (1)

#define BANANA_NUM          (3)
#define MONSTER_NUM         (5)


typedef struct {
   int x, y;
   int w, h;
   int ci;
   int dir;
   int dx;
   int dy;
} QUERY_DRAW;

static QUERY_DRAW MONSTER[MONSTER_NUM];
static QUERY_DRAW BANANA[BANANA_NUM];
static QUERY_DRAW MY;
static QUERY_DRAW gun;
static QUERY_DRAW gun2;
static QUERY_DRAW gun3;
static QUERY_DRAW gun4;

static QUERY_DRAW ui;
static QUERY_DRAW boss;

static unsigned short color[] = {RED, YELLOW, GREEN, BLUE, WHITE, BLACK};
static int MY_last_dir = 0;
static int level = 1; //무조건 1이상.
static int boss_hp = 280;
static int tick_count1 = 0;
static int tick_count2 = 0;
static int tick_count3 = 0;
static int tick_count4 = 0;
static int banana_flag = 0;
static int monster_flag =0;
static int skill_level = 0;
static int tracking_state = 0;
static int banana_crash[BANANA_NUM];


static int Check_my_Collision(QUERY_DRAW *obj);
static void banana_replace(void);

static void Draw_Object(QUERY_DRAW *obj)
{
   Lcd_Draw_Box(obj->x, obj->y, obj->w, obj->h, color[obj->ci]);
}

static int Check_Gun_Collision(QUERY_DRAW *obj)
{ 
    int kill_x = 0;
    int kill_y = 0;

    if ((gun.x >= obj->x) && ((obj->x + obj->w) >= gun.x)) kill_x++;
    else if ((gun.x < obj->x) && ((gun.x + gun.w) >= obj->x)) kill_x++;

    if ((gun.y >= obj->y) && ((obj->y + obj->h) >= gun.y)) kill_y++;
    else if ((gun.y < obj->y) && ((gun.y + gun.h) >= obj->y)) kill_y++;

    if (kill_x > 0 && kill_y > 0) {
        return 1;
    } else return 0;
}
static int Check_Gun_Collision2(QUERY_DRAW *obj,QUERY_DRAW *obj2)
{ 
    int kill_x = 0;
    int kill_y = 0;

    if ((obj2->x >= obj->x) && ((obj->x + obj->w) >= obj2->x)) kill_x++;
    else if ((obj2->x < obj->x) && ((obj2->x + obj2->w) >= obj->x)) kill_x++;

    if ((obj2->y >= obj->y) && ((obj->y + obj->h) >= obj2->y)) kill_y++;
    else if ((obj2->y < obj->y) && ((obj2->y + obj2->h) >= obj->y)) kill_y++;

    if (kill_x > 0 && kill_y > 0) {
        return 1;
    } else return 0;
}
static int Check_Monster_Collision(QUERY_DRAW *obj1,QUERY_DRAW *obj2 )
{ 
    int kill_x = 0;
    int kill_y = 0;

    if ((obj1->x >= obj2->x) && ((obj2->x + obj2->w) >= obj1->x)) kill_x++;
    else if ((obj1->x < obj2->x) && ((obj1->x + obj1->w) >= obj2->x)) kill_x++;

    if ((obj1->y >= obj2->y) && ((obj2->y + obj2->h) >= obj1->y)) kill_y++;
    else if ((obj1->y < obj2->y) && ((obj1->y + obj1->h) >= obj2->y)) kill_y++;

    if (kill_x > 0 && kill_y > 0) {
        return 1;
    } else return 0;
}

static int Check_banana_Collision(QUERY_DRAW *obj)
{ 
    int kill_x = 0;
    int kill_y = 0;

    if ((boss.x >= obj->x) && ((obj->x + obj->w) >= boss.x)) kill_x++;
    else if ((boss.x < obj->x) && ((boss.x + boss.w) >= obj->x)) kill_x++;

    if ((boss.y >= obj->y) && ((obj->y + obj->h) >= boss.y)) kill_y++;
    else if ((boss.y < obj->y) && ((boss.y + boss.h) >= obj->y)) kill_y++;

    if (kill_x > 0 && kill_y > 0) {
        return 1;
    } else return 0;
}

static int boss_move1(void)
{
   boss.ci = BACK_COLOR;
   Draw_Object(&boss);


      boss.dx= (rand() % 2) ? 1 : -1;
      boss.x += BOSS_STEP1 * boss.dx;

      // X축 경계 처리
      if ((boss.x < X_MIN) || (boss.x + boss.w > X_MAX)) {
         boss.dx = -boss.dx;
         if (boss.x < X_MIN) boss.x = X_MIN;
         if (boss.x + boss.w > X_MAX) boss.x = X_MAX - boss.w;
      }
      
      boss.ci = BOSS_COLOR1;
      Draw_Object(&boss);


   if(Check_Gun_Collision(&boss))
   {
      gun.ci = BACK_COLOR;
      Draw_Object(&gun);
      gun.x = 0; gun.y = 0;
      boss_hp--;
      Lcd_Draw_Box(boss_hp+40,0,1,16,color[5]);
   }
   
   if(Check_Gun_Collision2(&boss,&gun2))
   {
      gun2.ci = BACK_COLOR;
      Draw_Object(&gun2);
      gun2.x = 0; gun2.y = 0;
      boss_hp--; 
      Lcd_Draw_Box(boss_hp+40,0,1,16,color[5]);
   }
   if(Check_Gun_Collision2(&boss,&gun3))
   {
      gun3.ci = BACK_COLOR;
      Draw_Object(&gun3);
      gun3.x = 0; gun3.y = 0;
      boss_hp--;
      Lcd_Draw_Box(boss_hp+40,0,1,16,color[5]);
   }
   if(Check_Gun_Collision2(&boss,&gun4))
   {
      gun4.ci = BACK_COLOR;
      Draw_Object(&gun4);
      gun4.x = 0; gun4.y = 0;
      boss_hp--;
      Lcd_Draw_Box(boss_hp+40,0,1,16,color[5]);
   }

   if(Check_my_Collision(&boss))
   { 
   return 1;
   }
   return 0;
}
static int boss_move2(void)
{
   boss.ci = BACK_COLOR;
   Draw_Object(&boss);

      boss.x += BOSS_STEP2 * boss.dx;
      boss.y += BOSS_STEP2 * boss.dy;

      // X축 경계 처리
      if ((boss.x < X_MIN) || (boss.x + boss.w > X_MAX)) {
         boss.dx = -boss.dx;
         if (boss.x < X_MIN) boss.x = X_MIN;
         if (boss.x + boss.w > X_MAX) boss.x = X_MAX - boss.w;
      }
      
      // Y축 경계 처리
      if ((boss.y < Y_MIN + UI_SIZE_Y) || (boss.y + boss.h > Y_MAX)) {
         boss.dy = -boss.dy;
         if (boss.y < Y_MIN + UI_SIZE_Y) boss.y = Y_MIN + UI_SIZE_Y;
         if (boss.y + boss.h > Y_MAX) boss.y = Y_MAX - boss.h;
      }

      boss.ci = BOSS_COLOR2;
      Draw_Object(&boss);


   if(Check_Gun_Collision(&boss))
   {
      gun.ci = BACK_COLOR;
      Draw_Object(&gun);
      gun.x = 0; gun.y = 0;
      boss_hp--;
      Lcd_Draw_Box(boss_hp+40,0,1,16,color[5]);
   }
   if(Check_Gun_Collision2(&boss,&gun2))
   {
      gun2.ci = BACK_COLOR;
      Draw_Object(&gun2);
      gun2.x = 0; gun2.y = 0;
      boss_hp--; 
      Lcd_Draw_Box(boss_hp+40,0,1,16,color[5]);
   }
   if(Check_Gun_Collision2(&boss,&gun3))
   {
      gun3.ci = BACK_COLOR;
      Draw_Object(&gun3);
      gun3.x = 0; gun3.y = 0;
      boss_hp--;
      Lcd_Draw_Box(boss_hp+40,0,1,16,color[5]);
   }
   if(Check_Gun_Collision2(&boss,&gun4))
   {
      gun4.ci = BACK_COLOR;
      Draw_Object(&gun4);
      gun4.x = 0; gun4.y = 0;
      boss_hp--;
      Lcd_Draw_Box(boss_hp+40,0,1,16,color[5]);
   }

   if(Check_my_Collision(&boss))
   { 
   return 1;
   }
   return 0;

}
static void boss_rest(void)
{
   boss.ci = BOSS_COLOR3;
   Draw_Object(&boss);  
}
static void boss_wake_up(void)
{
   boss.ci = BOSS_COLOR2;
   boss.dx =1;
   boss.dy =1;
   Draw_Object(&boss);
}
static int boss_move3(void)
{
    // 상태를 유지하기 위한 static 변수들
     // 0이면 방향 탐색, 1이면 이동 중
    static int MY_last_x = 0;
    static int MY_last_y = 0;

    boss.ci = BACK_COLOR;
    Draw_Object(&boss);

    // 1. 처음 방향 설정
    if (tracking_state == 0)
    {
        MY_last_x = MY.x;
        MY_last_y = MY.y;

        // 보스 방향 설정 (1 또는 -1)
        boss.dx = (MY_last_x > boss.x) ? 1 : -1;
        boss.dy = (MY_last_y > boss.y) ? 1 : -1;

        tracking_state = 1; // 이동 상태로 전환
    }

    // 2. 방향대로 이동
    if(MY_last_x > boss.x)
    boss.x += BOSS_STEP3 * 1;
    if(MY_last_x < boss.x)
    boss.x -= BOSS_STEP3 * 1;
    
    if(MY_last_y > boss.y)
    boss.y += BOSS_STEP3 * 1;
    if(MY_last_y < boss.y)
    boss.y -= BOSS_STEP3 * 1;

   //  if((MY_last_y == boss.y) ||(MY_last_x == boss.x) )
   //  {
   //    boss.y += BOSS_STEP3 * boss.dx;
   //    boss.x += BOSS_STEP3 * boss.dy;
   //  }

    // 3. X축 경계 처리
    if ((boss.x < X_MIN) || (boss.x + boss.w > X_MAX)) {
        boss.dx = 0;
         
        if (boss.x < X_MIN) boss.x = X_MIN;
        if (boss.x + boss.w > X_MAX) boss.x = X_MAX - boss.w;
    }

    // 4. Y축 경계 처리
    if ((boss.y < Y_MIN + UI_SIZE_Y) || (boss.y + boss.h > Y_MAX)) {
        boss.dy = 0;
       
        if (boss.y < Y_MIN + UI_SIZE_Y) boss.y = Y_MIN + UI_SIZE_Y;
        if (boss.y + boss.h > Y_MAX) boss.y = Y_MAX - boss.h;
    }

    // 5. 다시 그리기
    boss.ci = 3;
    Draw_Object(&boss);

    // 6. 총알과 충돌 처리
    if (Check_Gun_Collision(&boss))
    {
        gun.ci = BACK_COLOR;
        Draw_Object(&gun);
        gun.x = 0; gun.y = 0;
        boss_hp--;
        Lcd_Draw_Box(boss_hp + 40, 0, 1, 16, color[5]);
    }

    if (Check_Gun_Collision2(&boss, &gun2))
    {
        gun2.ci = BACK_COLOR;
        Draw_Object(&gun2);
        gun2.x = 0; gun2.y = 0;
        boss_hp--;
        Lcd_Draw_Box(boss_hp + 40, 0, 1, 16, color[5]);
    }

    if (Check_Gun_Collision2(&boss, &gun3))
    {
        gun3.ci = BACK_COLOR;
        Draw_Object(&gun3);
        gun3.x = 0; gun3.y = 0;
        boss_hp--;
        Lcd_Draw_Box(boss_hp + 40, 0, 1, 16, color[5]);
    }

    if (Check_Gun_Collision2(&boss, &gun4))
    {
        gun4.ci = BACK_COLOR;
        Draw_Object(&gun4);
        gun4.x = 0; gun4.y = 0;
        boss_hp--;
        Lcd_Draw_Box(boss_hp + 40, 0, 1, 16, color[5]);
    }

    // 7. 플레이어와 충돌
    if (Check_my_Collision(&boss))
    {
        return 1; // 충돌 감지
    }

    return 0;
}


static void banana_reset(void)
{
   int i = 0;
   for(i=0; i<=2; i++)
   {
   banana_crash[i]=0;
   }
}
//바나나 설정하고 움직임까지
static int banana_move(void)
{
   int i;

    if(banana_flag == 0)// 처음 한번만 실행.
   {
   banana_replace();
   for (i = 0; i <= 2; i++) {
      BANANA[i].w = BANANA_SIZE_X;
      BANANA[i].h = BANANA_SIZE_Y;
      BANANA[i].ci = BANANA_COLOR;
      BANANA[i].dir = RIGHT;
      BANANA[i].dx = (rand() % 2) ? 1 : -1;
      BANANA[i].dy = (rand() % 2) ? 1 : -1;
    
   }
   banana_flag = 1;
   }
   
   for (i = 0; i <= 2; i++) 
   {
      BANANA[i].ci = BACK_COLOR;
      Draw_Object(&BANANA[i]);
  
      BANANA[i].x += BANANA_STEP * BANANA[i].dx;
      BANANA[i].y += BANANA_STEP * BANANA[i].dy;
  
      // X축 경계 맞추면 다시 안으로.
      if ((BANANA[i].x < X_MIN) || (BANANA[i].x + BANANA[i].w > X_MAX))
      {
      {
         BANANA[i].dx = -BANANA[i].dx;
         banana_crash[i]++;
      }
         if (BANANA[i].x < X_MIN) BANANA[i].x = X_MIN;
         if (BANANA[i].x + BANANA[i].w > X_MAX) BANANA[i].x = X_MAX - BANANA[i].w;
      }
      
      // Y축 경계 맞추면 다시 안으로
      if ((BANANA[i].y < Y_MIN + UI_SIZE_Y) || (BANANA[i].y + BANANA[i].h > Y_MAX)) 
      {
      {
         BANANA[i].dy = -BANANA[i].dy;
         banana_crash[i]++;
      }
         if (BANANA[i].y < Y_MIN + UI_SIZE_Y) BANANA[i].y = Y_MIN + UI_SIZE_Y;
         if (BANANA[i].y + BANANA[i].h > Y_MAX) BANANA[i].y = Y_MAX - BANANA[i].h;
      }
      if(banana_crash[i]>=4)
      {
      BANANA[i].ci = BACK_COLOR;
      Draw_Object(&BANANA[i]);
      BANANA[i].x = X_MAX;
      BANANA[i].y = Y_MAX;
      }
      if(Check_Gun_Collision(&BANANA[i]))
      {
         gun.ci = BACK_COLOR;
         Draw_Object(&gun);
         gun.x = 0; gun.y = 0;
      }
      if(Check_banana_Collision(&BANANA[i]))
      {
         BANANA[i].dy = -BANANA[i].dy;
         BANANA[i].dx = -BANANA[i].dx;
      }
      if(Check_my_Collision(&BANANA[i]))
      {
         return 1;
      }
      if(banana_crash[i]<4)
      {
      BANANA[i].ci = BANANA_COLOR;
      Draw_Object(&BANANA[i]);
      }

   }
   return 0;
}


static int Check_my_Collision(QUERY_DRAW *obj){
    int dead_x = 0;
    int dead_y = 0;

   if ((obj->x >= MY.x) && ((MY.x + MY.w) >= obj->x)) dead_x++;
    else if ((obj->x < MY.x) && ((obj->x + obj->w) >= MY.x)) dead_x++;
 
   if ((obj->y >= MY.y) && ((MY.y + MY.h) >= obj->y)) dead_y++;
    else if ((obj->y < MY.y) && ((obj->y + obj->h) >= MY.y)) dead_y++;

   if ((dead_x > 0) && (dead_y > 0)) 
   {
      return GAME_OVER;
   }
   return 0;
}

static int exp_up(void)
{
   if ( (ui.x >= X_MAX) || (ui.y>= Y_MAX) ) 
   {
    level +=1; 
    skill_level +=1;
    Lcd_Printf(0, 0, BLUE, WHITE, 1, 1, "lv");
    Lcd_Printf(20, 0, BLUE, WHITE, 1, 1, "%d", level);
    ui.ci = BACK_COLOR;
    Lcd_Draw_Box(40,0, 280, UI_SIZE_Y, color[5]);
    ui.x = 30; ui.y = 0; ui.w = UI_SIZE_X  ; ui.h = UI_SIZE_Y; ui.ci = UI_COLOR; ui.dir = RIGHT;

   Uart1_Printf("레벨 업!!\n");
   Uart1_Printf("총 업그레이드 !! 1번 거리, 속도 \n");
   }
    ui.ci = UI_COLOR;
    Draw_Object(&ui);
    ui.x += UI_SIZE_X; 
    return 0;
}

static int Gun_Shot(void) {
   gun.ci = BACK_COLOR;
   Draw_Object(&gun);

   if ((gun.x <= X_MIN || gun.x >= X_MAX || gun.y <= Y_MIN || gun.y >= Y_MAX)) 
   {
      gun.ci = BACK_COLOR;
      gun.x = 0; gun.y = 0;
      Draw_Object(&gun);
      return 0;
   }

   switch (gun.dir) 
   {
      case 0: gun.y -= GUN_STEP; break; 
      case 1: gun.y += GUN_STEP; break; 
      case 2: gun.x -= GUN_STEP; break; 
      case 3: gun.x += GUN_STEP; break; 
   }

   gun.ci = GUN_COLOR;
   Draw_Object(&gun);
   int i;
   if(level<5)
   {
   for (i = 0; i <=level; i++) 
   {
      if (Check_Gun_Collision(&MONSTER[i])) 
      {
         exp_up();
         MONSTER[i].ci = BACK_COLOR;
         Draw_Object(&MONSTER[i]);

         MONSTER[i].x =  rand() % (X_MAX - MONSTER_SIZE_X);
         MONSTER[i].y = (rand()%2) ? 30 :200;
         MONSTER[i].dx = 1;
         MONSTER[i].dy = (rand() % 2) ? 1 : -1;
         MONSTER[i].ci = MONSTER_COLOR; 
         gun.ci = BACK_COLOR;
         Draw_Object(&gun);
         gun.x = 0; gun.y = 0;
         Draw_Object(&MONSTER[i]);
      }
   }
   }
   return 0;
}
static int Gun_Shot2(void) {
   gun2.ci = BACK_COLOR;
   Draw_Object(&gun2);

   if ((gun2.x <= X_MIN || gun2.x >= X_MAX || gun2.y <= Y_MIN || gun2.y >= Y_MAX)) 
   {
      gun2.ci = BACK_COLOR;
      gun2.x = 0; gun2.y = 0;
      Draw_Object(&gun2);
      return 0;
   }

   switch (gun2.dir) 
   {
      case 0: gun2.y -= GUN_STEP; break; 
      case 1: gun2.y += GUN_STEP; break; 
      case 2: gun2.x -= GUN_STEP; break; 
      case 3: gun2.x += GUN_STEP; break; 
   }

   gun2.ci = GUN_COLOR;
   Draw_Object(&gun2);
   int i;
   if(level<5)
   {
   for (i = 0; i < level; i++) 
   {
      if (Check_Gun_Collision(&MONSTER[i])) 
      {
         exp_up();
         MONSTER[i].ci = BACK_COLOR;
         Draw_Object(&MONSTER[i]);

         MONSTER[i].x =  rand() % (X_MAX - MONSTER_SIZE_X);
         MONSTER[i].y = (rand()%2) ? 30 :200;
         MONSTER[i].dx = 1;
         MONSTER[i].dy = (rand() % 2) ? 1 : -1;
         MONSTER[i].ci = MONSTER_COLOR; 
         gun2.ci = BACK_COLOR;
         Draw_Object(&gun2);
         gun2.x = 0; gun2.y = 0;
         Draw_Object(&MONSTER[i]);
      }
   }
   }
   return 0;
}
static int Gun_Shot3(void) {
   gun3.ci = BACK_COLOR;
   Draw_Object(&gun3);

   if ((gun3.x <= X_MIN || gun3.x >= X_MAX || gun3.y <= Y_MIN || gun3.y >= Y_MAX)) 
   {
      gun3.ci = BACK_COLOR;
      gun3.x = 0; gun3.y = 0;
      Draw_Object(&gun3);
      return 0;
   }

   switch (gun3.dir) 
   {
      case 0: gun3.y -= GUN_STEP; break; 
      case 1: gun3.y += GUN_STEP; break; 
      case 2: gun3.x -= GUN_STEP; break; 
      case 3: gun3.x += GUN_STEP; break; 
   }

   gun3.ci = GUN_COLOR;
   Draw_Object(&gun3);
   int i;
   if(level<5)
   {
   for (i = 0; i < level; i++) 
   {
      if (Check_Gun_Collision(&MONSTER[i])) 
      {
         exp_up();
         MONSTER[i].ci = BACK_COLOR;
         Draw_Object(&MONSTER[i]);

         MONSTER[i].x =  rand() % (X_MAX - MONSTER_SIZE_X);
         MONSTER[i].y = (rand()%2) ? 30 :200;
         MONSTER[i].dx = 1;
         MONSTER[i].dy = (rand() % 2) ? 1 : -1;
         MONSTER[i].ci = MONSTER_COLOR; 
         gun3.ci = BACK_COLOR;
         Draw_Object(&gun3);
         gun3.x = 0; gun3.y = 0;
         Draw_Object(&MONSTER[i]);
      }
   }
   }
   return 0;
}
static int Gun_Shot4(void) {
   gun4.ci = BACK_COLOR;
   Draw_Object(&gun4);

   if ((gun4.x <= X_MIN || gun4.x >= X_MAX || gun4.y <= Y_MIN || gun4.y >= Y_MAX)) 
   {
      gun4.ci = BACK_COLOR;
      gun4.x = 0; gun4.y = 0;
      Draw_Object(&gun4);
      return 0;
   }

   switch (gun4.dir) 
   {
      case 0: gun4.y -= GUN_STEP; break; 
      case 1: gun4.y += GUN_STEP; break; 
      case 2: gun4.x -= GUN_STEP; break; 
      case 3: gun4.x += GUN_STEP; break; 
   }

   gun4.ci = GUN_COLOR;
   Draw_Object(&gun4);
   int i;
   if(level<5)
   {
   for (i = 0; i < level; i++) 
   {
      if (Check_Gun_Collision(&MONSTER[i])) 
      {
         exp_up();
         MONSTER[i].ci = BACK_COLOR;
         Draw_Object(&MONSTER[i]);

         MONSTER[i].x =  rand() % (X_MAX - MONSTER_SIZE_X);
         MONSTER[i].y = (rand()%2) ? 30 :200;
         MONSTER[i].dx = 1;
         MONSTER[i].dy = (rand() % 2) ? 1 : -1;
         MONSTER[i].ci = MONSTER_COLOR; 
         gun4.ci = BACK_COLOR;
         Draw_Object(&gun4);
         gun4.x = 0; gun4.y = 0;
         Draw_Object(&MONSTER[i]);
      }
   }
   }
   return 0;
}

static int MONSTER_Move(void) {
   int i;
   for (i = 0; i <= level; i++) {
       MONSTER[i].ci = BACK_COLOR;
       Draw_Object(&MONSTER[i]);

       
       if (i >= 3) {
           if (MY.x > MONSTER[i].x) MONSTER[i].dx = 1;
           else if (MY.x < MONSTER[i].x) MONSTER[i].dx = -1;
           else MONSTER[i].dx = 0;

           if (MY.y > MONSTER[i].y) MONSTER[i].dy = 1;
           else if (MY.y < MONSTER[i].y) MONSTER[i].dy = -1;
           else MONSTER[i].dy = 0;
       }

       // 이동
       MONSTER[i].x += MONSTER_STEP * MONSTER[i].dx;
       MONSTER[i].y += MONSTER_STEP * MONSTER[i].dy;

       // X축 경계 처리
       if ((MONSTER[i].x < X_MIN) || (MONSTER[i].x + MONSTER[i].w > X_MAX)) {
           MONSTER[i].dx = -MONSTER[i].dx;
           if (MONSTER[i].x < X_MIN) MONSTER[i].x = X_MIN;
           if (MONSTER[i].x + MONSTER[i].w > X_MAX) MONSTER[i].x = X_MAX - MONSTER[i].w;
       }

       // Y축 경계 처리
       if ((MONSTER[i].y < Y_MIN + UI_SIZE_Y) || (MONSTER[i].y + MONSTER[i].h > Y_MAX)) {
           MONSTER[i].dy = -MONSTER[i].dy;
           if (MONSTER[i].y < Y_MIN + UI_SIZE_Y) MONSTER[i].y = Y_MIN + UI_SIZE_Y;
           if (MONSTER[i].y + MONSTER[i].h > Y_MAX) MONSTER[i].y = Y_MAX - MONSTER[i].h;
       }

     
       int a, b;
       for (a = 0; a <= level; a++) {
           for (b = a + 1; b <= level; b++) 
           { 
               if (Check_Monster_Collision(&MONSTER[a], &MONSTER[b])) {
                   MONSTER[a].dx = -MONSTER[a].dx;
                   MONSTER[a].dy = -MONSTER[a].dy;
                   MONSTER[b].dx = -MONSTER[b].dx;
                   MONSTER[b].dy = -MONSTER[b].dy;
               }
           }
       }

       MONSTER[i].ci = MONSTER_COLOR;
       if(i>=3)
       {
         MONSTER[i].ci = 0;
       }
       Draw_Object(&MONSTER[i]);

       // 몬스터와 플레이어 충돌 확인
       if (Check_my_Collision(&MONSTER[i])) {
           return 1;
       }
   }

   return 0;
}

static void Game_Clear(void){
   Lcd_Clr_Screen();
   TIM4_Repeat_Interrupt_Enable(0, 0);
   TIM2_Repeat_Interrupt_Enable(0, 0);
   Uart_Printf("Game Clear, Please press any key to start.\n");
   Lcd_Printf(
      (LCDW - (8 * 10 * 2)) / 2,     // x 중앙 정렬 (10자 * 8px * 2배)
      LCDH / 2 - (16 * 3),           // y: 중앙보다 위로
      WHITE, BLACK, 2, 2,              // 글자 색, 배경색, x/배율
      "Game Clear"
   );
   Lcd_Printf(
      ((LCDW - (8 * 34 * 1)) / 2)+10,     // x 중앙 정렬 (34자 * 8px * 1배)
      LCDH / 2 + (16 * 0),           // y: Game Over 아래 약간
      WHITE, BLACK, 1, 1,            // 색상, 글자 배율
      "Please press any key to start."
   );
}
static void Game_Over(void){
   Lcd_Clr_Screen();
   TIM4_Repeat_Interrupt_Enable(0, 0);
   TIM2_Repeat_Interrupt_Enable(0, 0);
   Uart_Printf("Game Over, Please press any key to continue.\n");
   Lcd_Printf(
      (LCDW - (8 * 10 * 2)) / 2,     // x 중앙 정렬 (10자 * 8px * 2배)
      LCDH / 2 - (16 * 3)+10,           // y: 중앙보다 위로
      RED, BLACK, 2, 2,              // 글자 색, 배경색, x/배율
      "Game Over"
   );
   Lcd_Printf(
      ((LCDW - (8 * 34 * 1)) / 2) ,     // x 중앙 정렬 (34자 * 8px * 1배)
      LCDH / 2 + (16 * 0),           // y: Game Over 아래 약간
      WHITE, BLACK, 1, 1,            // 색상, 글자 배율
      "Please press any key to continue."
   );
}

static void Game_Start(void){
   Lcd_Clr_Screen();
   Lcd_Printf(
      (LCDW - (8 * 15 * 2)) / 2, // 15글자 * 8픽셀 * 2배율 → 가운데 정렬
      LCDH / 2 - (16 * 3),       // 수직 중앙보다 위쪽
      GREEN, BLACK, 2, 2,
      "Jungle Survival"
  );
  
  // 안내 문구 (작게, 아래쪽에)
  Lcd_Printf(
      ((LCDW - (8 * 26 * 1)) / 2)-20, // 26글자 * 8픽셀 * 1배율 → 가운데 정렬
      LCDH / 2 + (16 * 1),       // 중앙보다 아래
      WHITE, BLACK, 1, 1,
      "Press any key to start the game."
  );
}
static void boss_start(void){
   Lcd_Clr_Screen();
   TIM4_Repeat_Interrupt_Enable(0, 0);
   TIM2_Repeat_Interrupt_Enable(0, 0);
   Uart_Printf("WARNING BOSS APPEARED.\n");
   Lcd_Printf(
      ((LCDW - (8 * 10 * 2)) / 2)-20,     // x 중앙 정렬 (10자 * 8px * 2배)
      LCDH / 2 - (16 * 3),           // y: 중앙보다 위로
      RED, BLACK, 2, 2,              // 글자 색, 배경색, x/배율
      "WARING BOSS"
   );
   Lcd_Printf(
      ((LCDW - (8 * 34 * 1)) / 2) +20,     // x 중앙 정렬 (34자 * 8px * 1배)
      LCDH / 2 + (16 * 0),           // y: Game Over 아래 약간
      WHITE, BLACK, 1, 1,            // 색상, 글자 배율
      "Get ready for Boss phase."
   );
   Lcd_Printf(
      ((LCDW - (8 * 10 * 2)) / 2)-20,     // x 중앙 정렬 (10자 * 8px * 2배)
      LCDH / 2 - (16 * 3),           // y: 중앙보다 위로
      BLACK, BLACK, 2, 2,              // 글자 색, 배경색, x/배율
      "WARING BOSS"
   );
   Lcd_Printf(
      ((LCDW - (8 * 34 * 1)) / 2) +20,     // x 중앙 정렬 (34자 * 8px * 1배)
      LCDH / 2 + (16 * 0),           // y: Game Over 아래 약간
      BLACK, BLACK, 1, 1,            // 색상, 글자 배율
      "Get ready for Boss phase."
   );
}


static void k0(void) { if (MY.y > Y_MIN) MY.y -= MY_STEP; MY_last_dir = 0; }
static void k1(void) { if (MY.y + MY.h < Y_MAX) MY.y += MY_STEP; MY_last_dir = 1; }
static void k2(void) { if (MY.x > X_MIN) MY.x -= MY_STEP; MY_last_dir = 2; }
static void k3(void) { if (MY.x + MY.w < X_MAX) MY.x += MY_STEP; MY_last_dir = 3; }

static void MY_Move(int k) {
   static void (*key_func[])(void) = {k0, k1, k2, k3};
   if (k <= 3) key_func[k]();
}
static void FAKE_Move(int k) {
   static void (*key_func[])(void) = {k1, k0, k3, k2};
   if (k <= 3) key_func[k]();

}


static void Game_Init(void) {
   Lcd_Clr_Screen();
   MY.x = 150; MY.y = 220; MY.w = MY_SIZE_X; MY.h = MY_SIZE_Y; MY.ci = MY_COLOR; MY.dir = RIGHT;
   int i;
   for (i = 0; i < MONSTER_NUM; i++) {
      MONSTER[i].x = X_MIN + 10 * i;
      MONSTER[i].y = Y_MIN + UI_SIZE_Y + rand() % (Y_MAX - MONSTER_SIZE_Y - UI_SIZE_Y);
      MONSTER[i].w = MONSTER_SIZE_X;
      MONSTER[i].h = MONSTER_SIZE_Y;
      MONSTER[i].ci = MONSTER_COLOR;
      MONSTER[i].dir = RIGHT;
      MONSTER[i].dx = (rand() % 2) ? 1 : -1;
      MONSTER[i].dy = (rand() % 2) ? 1 : -1;
      // Lcd_Draw_Box(MONSTER[i].x, MONSTER[i].y, MONSTER[i].w, MONSTER[i].h, color[MONSTER[i].ci]);
   }
   boss.x = 120; boss.y = 40; boss.w = BOSS_SIZE_X; boss.h = BOSS_SIZE_Y; boss.ci = BOSS_COLOR1; boss.dx=1; boss.dy=1;boss.dir = LEFT;
   gun.x = MY.x+(MY.w-gun.w)/2; gun.y = MY.y-gun.h; gun.w = GUN_SIZE_X; gun.h = GUN_SIZE_Y; gun.ci = GUN_COLOR; gun.dir = MY_last_dir;
   gun2.w = GUN_SIZE_X; gun2.h = GUN_SIZE_Y; gun2.ci = GUN_COLOR; gun2.dir = MY_last_dir;
   gun3.w = GUN_SIZE_X; gun3.h = GUN_SIZE_Y; gun3.ci = GUN_COLOR; gun3.dir = MY_last_dir;
   gun4.w = GUN_SIZE_X; gun4.h = GUN_SIZE_Y; gun4.ci = GUN_COLOR; gun4.dir = MY_last_dir;
   ui.x = 40; ui.y = 0; ui.w = UI_SIZE_X; ui.h = UI_SIZE_Y; ui.ci = UI_COLOR; ui.dir = RIGHT;
   MY_last_dir = 0;
   Lcd_Draw_Box(MY.x, MY.y, MY.w, MY.h, color[MY.ci]);
}


static void gun_reload(void)
{
   gun.ci = BACK_COLOR;
   Draw_Object(&gun);
   
   gun.x = MY.x+(MY.w-gun.w)/2; gun.y = MY.y-gun.h; gun.w = GUN_SIZE_X; gun.h = GUN_SIZE_Y; gun.ci = GUN_COLOR; gun.dir = MY_last_dir;
}
static void gun_reload2(void)
{
   gun2.ci = BACK_COLOR;
   Draw_Object(&gun2);
   gun2.x = MY.x+(MY.w-gun2.w)/2 +5; gun2.y = MY.y-gun2.h +10  ; gun2.w = GUN_SIZE_X; gun2.h = GUN_SIZE_Y; gun2.ci = GUN_COLOR; gun2.dir = MY_last_dir;
}
static void gun_reload3(void)
{
   gun3.ci = BACK_COLOR;
   Draw_Object(&gun3);
   gun3.x = MY.x+(MY.w-gun3.w)/2 +10; gun3.y = MY.y-gun3.h +5 ; gun3.w = GUN_SIZE_X; gun3.h = GUN_SIZE_Y; gun3.ci = GUN_COLOR; gun3.dir = MY_last_dir;
}
static void gun_reload4(void)
{
   gun4.ci = BACK_COLOR;
   Draw_Object(&gun4);
   gun4.x = MY.x+(MY.w-gun4.w)/2-5; gun4.y = MY.y-gun4.h+10; gun4.w = GUN_SIZE_X; gun4.h = GUN_SIZE_Y; gun4.ci = GUN_COLOR; gun4.dir = MY_last_dir;
}

static void banana_replace(void) {
   if (boss.x >= 160 && boss.y < 120) { // 1사분면
       BANANA[0].x = boss.x;
       BANANA[0].y = boss.y + boss.h + 10;

       BANANA[1].x = boss.x - 10;
       BANANA[1].y = boss.y;

       BANANA[2].x = boss.x - 10;
       BANANA[2].y = boss.y + boss.h + 10;
   }
   else if (boss.x < 160 && boss.y < 120) { // 2사분면
       BANANA[0].x = boss.x;
       BANANA[0].y = boss.y + boss.h + 10;

       BANANA[1].x = boss.x + boss.w + 10;
       BANANA[1].y = boss.y;

       BANANA[2].x = boss.x + boss.w + 10;
       BANANA[2].y = boss.y + boss.h + 10;
   }
   else if (boss.x < 160 && boss.y >= 120) { // 3사분면
       BANANA[0].x = boss.x;
       BANANA[0].y = boss.y - 10;

       BANANA[1].x = boss.x + boss.w + 10;
       BANANA[1].y = boss.y;

       BANANA[2].x = boss.x + boss.w + 10;
       BANANA[2].y = boss.y - 10;
   }
   else if (boss.x >= 160 && boss.y >= 120) { // 4사분면
       BANANA[0].x = boss.x;
       BANANA[0].y = boss.y - 10;

       BANANA[1].x = boss.x - 10;
       BANANA[1].y = boss.y;

       BANANA[2].x = boss.x - 10;
       BANANA[2].y = boss.y - 10;
   }
}



extern volatile int TIM4_expired;
extern volatile int TIM3_expired;
extern volatile int TIM2_expired;
extern volatile int USART1_rx_ready;
extern volatile int USART1_rx_data;
extern volatile int Jog_key_in;
extern volatile int Jog_key;
extern volatile int SysTick_Flag;

void System_Init(void) {
   Clock_Init();
   LED_Init();
   Key_Poll_Init();
   Uart1_Init(115200);
   srand(0x1234);
   TIM3_Delay_Itr(1000);
   SysTick_OS_Tick(5);
   Lcd_Init(DIPLAY_MODE);
   Jog_Poll_Init();
   Jog_ISR_Enable(1);
   Uart1_RX_Interrupt_Enable(1);
   SCB->VTOR = 0x08003000;
   SCB->SHCSR = 7 << 16;
}

void Main(void) {

   int i = 0; //노래래
   System_Init();
   Uart_Printf("Game Start\n");
   // const char * note_name[] = {"C1", "C1#", "D1", "D1#", "E1", "F1", "F1#", "G1", "G1#", "A1", "A1#", "B1", "C2", "C2#", "D2", "D2#", "E2", "F2", "F2#", "G2", "G2#", "A2", "A2#", "B2"};
   const static unsigned short tone_value[] = {261,277,293,311,329,349,369,391,415,440,466,493,523,554,587,622,659,698,739,783,830,880,932,987};


   for (;;)
   {
      for(;;)
      {
      Game_Start();
      Jog_Wait_Key_Pressed();
      Jog_Wait_Key_Released();
      break;
      }
      Game_Init();
      TIM3_Out_Init();
      TIM4_Repeat_Interrupt_Enable(1, TIMER_PERIOD * 10);
      TIM3_Repeat_Interrupt_Enable(1, TIMER_PERIOD * 10);
      TIM2_Repeat_Interrupt_Enable(1, TIMER_PERIOD * 10);
      Lcd_Printf(0, 0, BLUE, WHITE, 1, 1, "lv");
      Lcd_Printf(20, 0, BLUE, WHITE, 1, 1, "%d", level);
      enum key{C1, C1_, D1, D1_, E1, F1, F1_, G1, G1_, A1, A1_, B1, C2, C2_, D2, D2_, E2, F2, F2_, G2, G2_, A2, A2_, B2};
      enum note{N16=BASE/4, N8=BASE/2, N4=BASE, N2=BASE*2, N1=BASE*4};
   
      const int song1[][2] = 
      {
      {G1_, N2}, {A1, N2}, {G1_, N2}, {F1, N2},
      {E1, N4}, {F1, N4}, {G1_, N2},
      {C1_, N2}, {D1, N2}, {C1_, N2},
      {G1, N1},
      {F1, N2}, {E1, N2}, {D1, N1},
      {G1_, N2}, {A1_, N2}, {G2, N1},
      // 여기서부터 확장 추가 (3배 채우기 위해)
      {F2, N2}, {F2_, N2}, {G2, N2},
      {D2, N4}, {C2_, N4}, {C2, N2},
      {B1, N2}, {A1_, N2}, {G1_, N2},
      {G1, N1}, {F1, N2}, {E1, N2},
      {D1, N1},
      {G1_, N2}, {G1, N2}, {F1, N1}
      };

      int lose_hp = HP_SIZE;
      int gun_range = 500;
      int gun_level = 1;
      int boss_event = 0;
      boss_hp =280;
      level=1;
      skill_level=0;
      int confuse_flag =0;\
      int boss_move_flag =0;
      for (;;)
      {
         int game_clear =0;
         int game_over = 0;
         int hurt = 0;
         int boss_flag = 0;
         
       
         

         if (Jog_key_in) 
         {
            if(confuse_flag ==1)
            {
               MY.ci = BACK_COLOR;
               Draw_Object(&MY);
               FAKE_Move(Jog_key);
               MY.ci = MY_CONFUSE_COLOR;
               Draw_Object(&MY); 
            }
            if(confuse_flag==0)
            {
            MY.ci = BACK_COLOR;
            Draw_Object(&MY);
            MY_Move(Jog_key);
            MY.ci = MY_COLOR;
            Draw_Object(&MY);
            }
            Jog_key_in = 0;
            
         }

         if (TIM4_expired) 
         {
            if(level<5)
            {
             hurt= MONSTER_Move();
            }
            else if(monster_flag == 0)
            {
               int i;
               for(i=0; i<MONSTER_NUM;i++)
               {
                  MONSTER[i].ci =BACK_COLOR;
                  Draw_Object(&MONSTER[i]);
                  MONSTER[i].x=0;
                  MONSTER[i].y=0;
               }
               monster_flag = 1;
            }
            if(level == 5 && boss_event==0)
            {
               boss_event=1;
               boss_start();
               boss_start();
               boss_start();
               boss_start();
               boss_start();
               boss_start();
               boss_start();
               boss_start();
               boss_start();
               boss_start();
               Lcd_Draw_Box(40,0,280,15,color[0]);
               Lcd_Printf(0, 0, BLUE, WHITE, 1, 1, "boss");
               Draw_Object(&MY);
               
            }

            if(level>=5)
            {  
               tick_count2 += TIMER_PERIOD*5;
               if(boss_hp<=PHASE2)
               {
                  tick_count3 += TIMER_PERIOD*5;
               }
               if(boss_hp<=PHASE3)
               {
                  tick_count4 += TIMER_PERIOD*5;
               }
               if(boss_flag == 0)
               {
               boss_flag = 1;
               TIM4_Repeat_Interrupt_Enable(1, TIMER_PERIOD*5);
               }
               if(boss_hp>=PHASE2 && level >=5)
               {
                  hurt = boss_move1();
               }
               if(boss_hp>=0 && boss_hp<PHASE2 && boss_move_flag==0)
               {
                  hurt = boss_move2();
               }
               if(tick_count2 >= 10000)
               {
               hurt = banana_move();
               }
               if(tick_count2 >=20000)
               {
                  banana_reset();
                  int z;
                  for(z=0; z<=2; z++)
                  {
                     BANANA[z].ci =BACK_COLOR;
                     Draw_Object(&BANANA[z]);
                     BANANA[z].x =0;
                     BANANA[z].y =0;
                  }
                  banana_flag = 0;
                  tick_count2 = 0;
               }
               if(tick_count3 >= 10000)
               {
                  confuse_flag = 1;
               }
               if(tick_count3 >= 15000)
               {
                  confuse_flag = 0;
                  tick_count3 = 0;
               }
               
               if(tick_count4< 12000 && tick_count4 >= 10001)
               {
                  boss_rest();
                  boss_move_flag = 1;
               }
               if(tick_count4 > 12000 )
               {
                  hurt = boss_move3();
               }

               if(tick_count4>=17000)
               {
                  boss_wake_up();
                  tick_count4 = 0;
                  tracking_state = 0;
                  boss_move_flag = 0;
               }
               
               if(boss_hp<=0)
               {
                  game_clear=1;
               }

            }
            TIM4_expired = 0;
         }

      
         if(TIM3_expired)
         {
            TIM3_expired = 0;
         }
            
         if(SysTick_Flag)
         { 
         tick_count1 += TIMER_PERIOD;
         
         if(tick_count1 >= gun_range)
         {
            gun_reload();
            if(gun_level>= 2)
            {
               gun_reload2();
            }
            if(gun_level>= 3)
            {
               gun_reload3();
            }
            if(gun_level>= 4)
            {
               gun_reload4();
            }
            tick_count1 = 0;
         }
      

         Gun_Shot();
         if(gun_level>= 2)
         {
            Gun_Shot2();
         }
         if(gun_level>= 3)
         {
            Gun_Shot3();
         }
         if(gun_level>= 4)
         {
            Gun_Shot4();
         }


         SysTick_Flag = 0;
         }

         if(TIM2_expired)
         {
            TIM3_Out_Freq_Generation(tone_value[song1[i][0]]);
            TIM2_Delay2(song1[i][1]);
            i++;

            if(i>((sizeof(song1)/sizeof(song1[0])))-1)
            {
               i=0;
            }

            TIM2_expired=0;
         }

         if (USART1_rx_ready) 
         {  if(USART1_rx_data == '1' && skill_level!=0)
            {
               skill_level --;
               gun_range += 100;
               Uart1_Printf("총알 사정거리 증가!\n");
               Uart1_Printf("현재 총알 사거리, %d\n", gun_range);
               Uart1_Printf("남은 스킬 포인트 : %d \n", skill_level);
            }
            if(USART1_rx_data =='2' && skill_level!=0)
            {
               if(gun_level<=4)
               {
               skill_level --;
               gun_level += 1;
               Uart1_Printf("총알 개수 증가!\n");
               Uart1_Printf("현재 총알 개수: %d\n", gun_level);
               Uart1_Printf("남은 스킬 포인트 : %d \n", skill_level);
               }
            }
            if(USART1_rx_data == 'm'||USART1_rx_data == 'M')
            {
               Macro_Write_Block(GPIOB->CRL,0xf,0x0,0);
            }
            if(USART1_rx_data == 'p'||USART1_rx_data == 'P')
            {
               Macro_Write_Block(GPIOB->CRL,0xf,0xb,0);
            }
            if(USART1_rx_data == 'e'||USART1_rx_data == 'E')
            {
               skill_level++;
               gun_level++;
            }
            if(USART1_rx_data == 'b'||USART1_rx_data == 'B')
            {
               game_clear = 1;
            }
            if(USART1_rx_data == '7')
            {
               boss_hp -=10;
               Lcd_Draw_Box(boss_hp + 40, 0, 10, 16, color[5]);
            }
            

            USART1_rx_ready= 0;
         }

         if(hurt)
         {
            lose_hp --;
            MY.ci=BACK_COLOR;
            Draw_Object(&MY);
            MY.x =  rand() % (X_MAX - MY_SIZE_X-5);
            MY.y = (rand()%2) ? 40 :120;
            MY.ci = MY_COLOR;
            Draw_Object(&MY);
            Uart1_Printf("몬스터에게 피해를 입었습니다\n");
            Uart1_Printf("남은 hp는 : %d \n", lose_hp);
            hurt=0;
            if(lose_hp==0)
            {
               game_over = 1;
            }
         }
         
         
        

         if(game_over) 
         {
            Game_Over();
            Jog_Wait_Key_Pressed();
            Jog_Wait_Key_Released();
            level=0;
            Uart_Printf("Game Over, Please press any key to continue.\n");
            break;
         }

         if(game_clear) 
         {
            Game_Clear();
            Jog_Wait_Key_Pressed();
            Jog_Wait_Key_Released();
            level=0;
            Uart_Printf("Game Clear, Please press any key to start.\n");
            break;
         }
      }
   }
}



