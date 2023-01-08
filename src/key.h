#ifndef KEY_H
#define KEY_H

void key_init(void* arg);
int key_getstate(int key);
void* key_poll(void* arg);

#endif