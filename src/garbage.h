#ifndef _GARBAGE_H
#define _GARBAGE_H 1

typedef struct {
    char *std = 0;
    char *paper = 0;
    char *image = 0;
} GarbageDays;

GarbageDays check_garbage(struct tm timeinfo);
void free_garbage(GarbageDays*);
#endif