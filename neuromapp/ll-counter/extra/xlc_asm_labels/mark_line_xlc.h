#ifndef GGG
#define GGG

#define AMXLC_PASTE5(a,b,c,d,e) a ## b ## c ## d ## e
#define AMXLC_TBL_NAME(x,y) AMXLC_PASTE5(amxlc_tbl__,x,__,y,__endentry)


#define ASM_MARK_XLC_(label,spr) \
{ static int AMXLC_TBL_NAME(label,__LINE__) __attribute__((section(".amxlc_labels"))); }\
asm volatile (  "b 1f\n\t" "mtspr " spr ",0\n\t" "addi 3,0,%0\n\t1:\n\t" :: "i"(__LINE__) : "memory" );

#define ASM_MARK_XLC_FROM(label) ASM_MARK_XLC_(label,"808")
#define ASM_MARK_XLC_TO(label) ASM_MARK_XLC_(label,"809")

#endif // ndef GGG
