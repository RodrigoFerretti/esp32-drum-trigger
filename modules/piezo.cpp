struct piezo_t
{
    int sample;
    int pin;
};

piezo_t piezo_left = {
    .sample = 0,
    .pin = 33,
};

piezo_t piezo_right = {
    .sample = 0,
    .pin = 34,
};
