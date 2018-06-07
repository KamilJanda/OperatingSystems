#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

struct agrumentData
{
    int numberOfThread;
    char *inPictureFile;
    char *filterFileName;
    char *outFileName;
};

struct imageData
{
    int **image;
    int width;
    int heigth;
    int max;
};

struct filterData
{
    double **filter;
    int size;
};

struct imageData *IMAGE_IN;
struct imageData *IMAGE_OUT;
struct filterData *FILTER;
int NUMBER_OF_THREADS;

struct agrumentData *parse(int argc, char *argv[])
{
    if (argc != 5)
        perror("Wrong numer of arguments /n");

    struct agrumentData *result = malloc(sizeof(struct agrumentData));

    result->numberOfThread = atoi(argv[1]);
    NUMBER_OF_THREADS = atoi(argv[1]);
    result->inPictureFile = argv[2];
    result->filterFileName = argv[3];
    result->outFileName = argv[4];

    return result;
}

struct imageData *load_image(char *path)
{
    struct imageData *result = malloc(sizeof(struct imageData));

    FILE *file = fopen(path, "r");
    fscanf(file,"P2 %d %d %d",&result->width,&result->heigth,&result->max);

    result->image = calloc(result->heigth, sizeof(int *));

    for (int i = 0; i < result->heigth; i++)
    {
        result->image[i] = calloc(result->width, sizeof(int));

        for (int j = 0; j < result->width; ++j)
        {
            fscanf(file, "%d ", &result->image[i][j]);
        }
    }

    fclose(file);
    return result;
}

struct filterData *load_filter(char *path)
{
    struct filterData *result = malloc(sizeof(struct filterData));

    FILE *file = fopen(path, "r");
    fscanf(file, "%d\n", &result->size);

    result->filter = calloc(result->size, sizeof(double *));

    for (int i = 0; i < result->size; ++i)
    {
    
        result->filter[i] = calloc(result->size, sizeof(double));
        for (int j = 0; j < result->size; ++j)
        {
            fscanf(file, "%lf ", &result->filter[i][j]);
        }
    }

    fclose(file);
    return result;
}

void save_image(char *path)
{
    FILE* file = fopen(path, "w");
    fprintf(file, "P2\n%d %d\n%d\n", IMAGE_OUT->width, IMAGE_OUT->heigth, IMAGE_OUT->max);

    for (int i = 0; i < IMAGE_OUT->heigth; ++i) {
        for (int j = 0; j < IMAGE_OUT->width; ++j) {
            fprintf(file, "%d ", IMAGE_OUT->image[i][j]);
        }
    }

    fprintf(file, "\n");
}

struct imageData *init_out_image()
{
    struct imageData *result = malloc(sizeof(struct imageData));
    result->heigth = IMAGE_IN->heigth;
    result->width = IMAGE_IN->width;
    result->max = IMAGE_IN->max;

    result->image = calloc(result->heigth, sizeof(int *));

    for (int i = 0; i < result->heigth; i++)
    {
        result->image[i] = calloc(result->width, sizeof(int));
    }

    return result;
}

// HELPERS

int max(int a, int b){
    return a>b ? a : b;
}

int min(int a, int b){
    return a > b ? b : a;
}

void print_img()
{
    for (int i = 0; i < IMAGE_OUT->heigth; ++i) {
        for (int j = 0; j < IMAGE_OUT->width; ++j) {
            printf("%d ", IMAGE_OUT->image[i][j]);
        }
        printf("\n");
    }
}


//////////////////////////////

int compute_pixel(int x, int y)
{
    double sum = 0;
    for (int i = 0; i < FILTER -> size; i++)
    {
        for (int j = 0; j < FILTER -> size; j++)
        {
            int ix = min(max(0, x - ceil(FILTER -> size / 2) + i), IMAGE_IN->heigth - 1);
            int iy = min(max(0, y - ceil(FILTER -> size / 2) + j), IMAGE_IN->width - 1);
            sum += IMAGE_IN->image[ix][iy] * FILTER->filter[i][j];
        }
    }

    return (int) lround(sum);
}

void compute_batch(void* args)
{
    int n = *(int *)args;
    int start, end;
    start = IMAGE_IN->heigth * n / NUMBER_OF_THREADS;
    end = IMAGE_IN->heigth * (n+1) / NUMBER_OF_THREADS;

    for(int i=start; i<end; i++){
        for(int j=0; j<IMAGE_IN->width; j++){
            IMAGE_OUT->image[i][j] = compute_pixel(i, j);
        }
    }

    pthread_exit(args);
}

void process_image()
{

    pthread_t *thread = malloc(sizeof(pthread_t) * NUMBER_OF_THREADS);
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        int *arg = malloc(sizeof(int));
        *arg = i;
        pthread_create(&(thread[i]), NULL,(void* (*)(void*)) compute_batch, arg);
    }

    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        void *ptr;
        pthread_join(thread[i], &ptr);
    }


    free(thread);
}


// --- MAIN ---

int main(int argc, char *argv[])
{
    struct agrumentData *programData = parse(argc, argv);

    IMAGE_IN = load_image(programData->inPictureFile);
    IMAGE_OUT = init_out_image();
    FILTER = load_filter(programData->filterFileName);

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_REALTIME, &start_time);

    process_image();

    clock_gettime(CLOCK_REALTIME, &end_time);

    unsigned long long ms = (end_time.tv_sec - start_time.tv_sec) * 1000000;
    ms += (end_time.tv_nsec - start_time.tv_nsec) / 1000;
    printf("number of threads: %d,ms: %llu\n", programData->numberOfThread,  ms);

    save_image(programData->outFileName);

    return 0;
}