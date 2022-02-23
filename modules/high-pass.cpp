struct high_pass_t
{
    int EMA_s;
    float EMA_a;

    void filter_sample(int *sample)
    {
        EMA_s = (EMA_a * *sample) + ((1 - EMA_a) * EMA_s);

        *sample = *sample - EMA_s;
    };
};

high_pass_t high_pass_piezo_left = {
    .EMA_s = 0,
    .EMA_a = 0.04,
};

high_pass_t high_pass_piezo_right = {
    .EMA_s = 0,
    .EMA_a = 0.04,
};
