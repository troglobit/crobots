/* declare the intrinsic functions, all must push a long value on the stack */
/* these functions don't return a long, but declared long for notation */
void c_scan(void);    /* scan(degree,res);  >0 = robot distance, 0 = nothing */
void c_cannon(void);  /* cannon(degree,dist); fire cannon */
void c_drive(void);   /* drive(degree,speed); speed 0-100 in % */
void c_damage(void);  /* damage(); = current damage in % */
void c_speed(void);   /* speed(); = current speed */ 
void c_loc_x(void);   /* loc_x(); = current x location */
void c_loc_y(void);   /* loc_y(); = current y location */
void c_rand(void);    /* rand(limit); = 0 -- limit (2**15)-1 */
void c_sin(void);     /* sin(degree); = sin * 100000 */
void c_cos(void);     /* cos(degree); = cos * 100000 */
void c_tan(void);     /* tan(degree); = tan * 100000 */
void c_atan(void);    /* atan(ratio); = degree */
void c_sqrt(void);    /* sqrt(x); = square root */