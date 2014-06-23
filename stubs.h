static inline long millis(void)
{
        return 0;
}

static inline void puts_gps(const char *str)
{
        (void)str;
}

static inline void putc_gps(const char c)
{
        (void)c;
}

static inline uint8_t getc_gps(void)
{
        return '0';
}

static inline bool gps_encode(char c)
{
        (void)c;
        return true;
}
