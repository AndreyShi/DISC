

#define cli()
#define sei()

#define ISR(x) void isr_##x()

#ifdef __cplusplus
extern "C"{
extern void isr_RTC_OVF_vect();
}
#endif
