extern "C"
{
void setIrq(void (*irq)(void), int enable);
int sti(int);
}
