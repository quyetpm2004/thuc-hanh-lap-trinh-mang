#include <stdio.h>

struct student
{
    char name[20];
    int eng;
    int math;
    int phys;
    double mean;
};

static struct student data[] = {
    {"Tuan", 82, 72, 58, 0.0},
    {"Nam", 77, 82, 79, 0.0},
    {"Khanh", 52, 62, 39, 0.0},
    {"Phuong", 61, 82, 88, 0.0}};

void calculate_mean(struct student *s)
{
    s->mean = (s->eng + s->math + s->phys) / 3.0;
}

char rank_student(struct student *s)
{
    if (s->mean >= 90 && s->mean <= 100)
    {
        return 'S';
    }
    else if (s->mean >= 80 && s->mean < 90)
    {
        return 'A';
    }
    else if (s->mean >= 70 && s->mean < 80)
    {
        return 'B';
    }
    else if (s->mean >= 60 && s->mean < 70)
    {
        return 'C';
    }
    else if (s->mean < 60)
    {
        return 'D';
    }
    else
    {
        return 'X'; // Invalid mean value
    }
}

typedef struct student2
{
    char name[20];
    int eng;
    int math;
    int phys;
} STUDENT;
STUDENT data2[] = {
    {"Tuan", 82, 72, 58},
    {"Nam", 77, 82, 79},
    {"Khanh", 52, 62, 39},
    {"Phuong", 61, 82, 88}};

STUDENT *p = data2;

int main()
{
    int i;
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
    {
        calculate_mean(&data[i]);
    }

    printf("Cau a, b: Tinh diem TB va rank\n");
    printf("Name\tEnglish\tMath\tPhysics\tMean\tRank\n");
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
    {
        printf("%s\t%d\t%d\t%d\t%.2f\t%c\n", data[i].name, data[i].eng, data[i].math, data[i].phys, data[i].mean, rank_student(&data[i]));
    }

    printf("\n\nCau c: Duyet mang bang con tro p\n");
    printf("Name\tEnglish\tMath\tPhysics\n");

    for (int i = 0; i < sizeof(data2) / sizeof(data2[0]); i++)
    {
        printf("%s\t%d\t%d\t%d\n", p[i].name, p[i].eng, p[i].math, p[i].phys);
    }

    return 0;
}