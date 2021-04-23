#include "scheduler.h"

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Macro to silence gcc
#define UNUSED(VAR) ((void) (VAR))

/**************************************************************************
 *                                                                        *
 *                                SETTINGS                                *
 *                                                                        *
 * You can modify the value of constansts or the definition of functions. *
 *                                                                        *
 *************************************************************************/

// Size of the buffer used while reading the input.
static const size_t LINE_BUFFER_LIMIT = 4096;

// Maximum number of queues you can create with command <create>.
static const size_t QUEUES_LIMIT = 16;

/* Number of <process id> available for commands <push>, <print>,
 * <renice> and <sum>.
 * The range of process ids is <0, PROCESSES_LIMIT - 1>.
 * 
 * DISCLAIMER: 
 *   Calling <print> or <sum> with <process id>, which was
 *   pushed to a queue is undefined behaviour and can cause          
 *   an invalid access to the memory.
 */
static const unsigned PROCESSES_LIMIT = 32;

// Maximum count of numeric arguments for command <sum>.
static const size_t SUM_NUMBERS_LIMIT = 32;

/**
 * Callback, which is set for every process by default.
 * 
 * @note Feel free to modify it for your own testing.
 * 
 * @param time       a run time of the proccess
 * @param context    always NULL
 * @return unsigned  0 if finished; otherwise an estimated time increase
 */
static unsigned default_callback(unsigned time, void *context)
{
    UNUSED(time);
    UNUSED(context);

    return 0;
}



/************************************************************************
 *                                                                      *
 *                        IMPLEMENTATION SECTION                        *
 *                                                                      *
 * If you want to have working commands <push> and <sum>, then you      *
 * have to implement them yourself.                                     *
 *                                                                      *
 * Bellow you can see two triplets of functions:                        *
 * <create_X_context>, <X_callback> and <free_X_context>,               *
 * where X ∈ {sum, print}.                                              *
 *                                                                      *
 * <create_X_context> will create a context which will be passed as a   *
 * second argument to <X_callback>. The context contains the data       *
 * required for callback to function properly.                          *
 * <free_X_context> frees any resources allocated in <create_X_context> *
 *                                                                      *
 * The documentation of <X_callback> describes the recommended          *
 * implemenation, which you can follow. However, feel free to modify it *
 * to one's needs.                                                      *
 *                                                                      *
 * Hints:                                                               *
 *  - you can define your own helper functions and data structures.     *
 *  - you can use flexible array member (FAM)                           *
 *                                                                      *
 ***********************************************************************/

typedef struct num_array {
    size_t length;
    int* numbers;
} num_array;

/**
 * Creates a context for <sum_callback>.
 * 
 * @note Don't assume anything about the life time of @c numbers array,
 *       i.e, data pointed by @c numbers could not be in the memory after
 *       this function returns.
 *       
 * @param n        count of elements of the @c numbers
 * @param numbers  a pointer to an array containing arguments of the <sum> command
 * @return void*   a pointer to a context or
 *                   NULL if an error ocurred
 */
static void *create_sum_context(size_t n, int numbers[n])
{
    UNUSED(n);
    UNUSED(numbers);
    return NULL;
}

/**
 * Computes the sum of numbers.
 * 
 * Recommended implementation:
 *  - it takes exactly one unit of time to add two numbers
 *  - each call to the function prints to the standard output
 *    the current sum.
 * 
 * @param time       a run time of the process
 * @param context    a pointer to a context returned by <create_sum_context>
 * @return unsigned  0 if finished; otherwise an estimated time increase
 */
static unsigned sum_callback(unsigned time, void *context)
{
    UNUSED(time);
    UNUSED(context);
    return 0;
}

/**
 * Frees any resources allocated in <create_sum_context>
 * 
 * @note if <create_sum_context> doesn't allocate anything, you 
 *       don't need to make any changes to this function
 * 
 * @param context a pointer to a context returned by <create_sum_context>
 */
static void free_sum_context(void *context)
{
    UNUSED(context);

    /* TODO */
}

/**
 * Creates a context for <print_callback>.
 * 
 * @note Don't assume anything about the life time of @c string, i.e, data
 *       pointed by @c string could not be in the memory after
 *       this function returns.
 * 
 * @param string a pointer to null-terminated byte string,
 *                 given as an argument of <print> command
 * @return void* a pointer to a context or
 *                 NULL if an error ocurred
 */
static void *create_print_context(const char *string)
{
    UNUSED(string);

    /* TODO */

    return NULL;
}

/**
 * Prints all the characters of the string
 * 
 * Recommended implementation:
 *  - it takes exactly one unit of time to print a character
 * 
 * @param time       a run time of the process
 * @param context    a pointer to a context returned by <create_print_context>
 * @return unsigned  0 if finished; otherwise an estimated time increase
 */
static unsigned print_callback(unsigned time, void *context)
{
    UNUSED(time);
    UNUSED(context);

    /* TODO */

    return 0;
}

/**
 * Frees any resources allocated in <create_print_context>
 * 
 * @note if <create_print_context> doesn't allocate anything, you 
 *       don't need to make any changes to this function
 * 
 * @param context a pointer to a context returned by <create_print_context>
 */
static void free_print_context(void *context)
{
    UNUSED(context);

    /* TODO */
}

/*****************************************************************************
 *                                                                           *
 *                      DO NOT MODIFY THE CODE BELLOW!!                      *
 *                                                                           *
 *                                  ⁽ᴼʳ ᵈᵒ⁾                                  *
 *                             ⁽ᴵ ᵃᵐ ʲᵘˢᵗ ᵃ ᵗᵉˣᵗ⁾                            *
 *                                                                           *
 ****************************************************************************/

enum status_code
{
    STATUS_OK,
    STATUS_STOP,
    STATUS_ERROR,
};

typedef struct named_queue
{
    char *name;
    priority_queue queue;
} named_queue;

typedef struct arguments
{
    size_t queues_count;
    priority_queue *queues[2];
    unsigned proccess_id;
    unsigned time;
    unsigned niceness;
    uint16_t cpu_mask;
    char *string;

    size_t numbers_count;
    int *numbers;

} arguments;

typedef struct state
{
    size_t queue_count;
    named_queue *queues;

    size_t process_count;
    process_type *processes;

    const char *word_delims;
    char **mandatory_args;
    arguments curr_args;
} state;

static void print_pid(size_t pid)
{
    printf("pid: %zu\n", pid);
}

typedef struct context_wrapper
{
    size_t pid;
    void *user_context;
    void (*free_callback)(void *);
} context_wrapper;

static size_t get_pid(context_wrapper *cw)
{
    return cw->pid;
}

static void set_pid(context_wrapper *cw, size_t pid)
{
    cw->pid = pid;
}

static unsigned callback_wrapper(
        unsigned time, context_wrapper *cw, cb_type callback)
{
    print_pid(get_pid(cw));
    return callback(time, cw->user_context);
}

static unsigned _default_callback(unsigned time, void *context)
{
    return callback_wrapper(time, context, default_callback);
}

static unsigned _sum_callback(unsigned time, void *context)
{
    return callback_wrapper(time, context, sum_callback);
}

static unsigned _print_callback(unsigned time, void *context)
{
    return callback_wrapper(time, context, print_callback);
}

static bool str_to_num(
        const char *num_str, long long *num, long long min, long long max)
{
    char *end_ptr;
    errno = 0;
    *num = strtoll(num_str, &end_ptr, 0);
    if (num_str == end_ptr || *end_ptr != '\0' || errno != 0 || *num < min
            || *num > max) {
        return false;
    }
    return true;
}

static bool str_to_int(const char *num_str, int *number)
{
    long long num;
    if (!str_to_num(num_str, &num, INT_MIN, INT_MAX)) {
        printf("cannot convert '%s' to a integer\n", num_str);
        return false;
    }
    *number = (int) num;
    return true;
}

static bool str_to_unsigned(const char *num_str, unsigned *number)
{
    long long num;
    if (!str_to_num(num_str, &num, 0, LONG_MAX)) {
        printf("cannot convert: '%s' to an unsigned number\n", num_str);
        return false;
    }
    *number = (unsigned) num;
    return true;
}

static char *next_word(state *state)
{
    return strtok(NULL, state->word_delims);
}

static void print_missing(const char *arg_name)
{
    printf("missing arg: <%s>\n", arg_name);
}

static void print_not_found(const char *queue_name)
{
    printf("queue named \"%s\" does not exist\n", queue_name);
}

static void print_bool_result(bool result)
{
    printf("return value: %s\n", result ? "true" : "false");
}

static named_queue *find_queue_by_name(const char *queue_name, state *state)
{
    for (size_t i = 0; i < state->queue_count; i++) {
        named_queue *nq = &state->queues[i];
        if (strcmp(nq->name, queue_name) == 0) {
            return nq;
        }
    }
    return NULL;
}

static bool reset_process(process_type *p, size_t pid)
{
    context_wrapper *cw = malloc(sizeof(*cw));
    if (cw == NULL) {
        printf("the memory is exhausted\n");
        return false;
    }

    cw->user_context = NULL;
    cw->free_callback = NULL;
    set_pid(cw, pid);
    p->context = cw;
    p->callback = _default_callback;
    return true;
}

static char *str_dup(const char *str)
{
    assert(str != NULL);
    size_t len = strlen(str);

    char *copy = malloc((len + 1) * sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    return strcpy(copy, str);
}

static enum status_code create_new_queue(char *name, state *state)
{
    if (state->queue_count == QUEUES_LIMIT) {
        printf("queue limit reached\n");
        return STATUS_STOP;
    }

    char *name_copy = str_dup(name);
    if (name_copy == NULL) {
        fprintf(stderr, "the memory is exhausted\n");
        return STATUS_ERROR;
    }

    state->queues[state->queue_count].name = name_copy;
    state->queues[state->queue_count].queue = create_queue();
    ++state->queue_count;
    return STATUS_OK;
}

static enum status_code create_handler(state *state)
{
    enum status_code code;
    if ((code = create_new_queue(state->mandatory_args[0], state))
            == STATUS_ERROR) {
        return STATUS_ERROR;
    }
    if (code == STATUS_OK) {
        printf("queue created\n");
    }
    return STATUS_OK;
}

static enum status_code copy_handler(state *state)
{
    bool result = copy_queue(
            state->curr_args.queues[0], state->curr_args.queues[1]);
    print_bool_result(result);
    return STATUS_OK;
}

static enum status_code clear_handler(state *state)
{
    clear_queue(state->curr_args.queues[0]);
    printf("queue cleared\n");
    return STATUS_OK;
}

static const char *PUSH_RESULT_DICT[] = {
    "push_success",
    "push_duplicate",
    "push_inconsistent",
    "push_error",
};

static enum status_code push_handler(state *state)
{
    process_type *p = &state->processes[state->curr_args.proccess_id];
    process_type process_arg = *p;
    process_arg.remaining_time = state->curr_args.time;
    process_arg.niceness = state->curr_args.niceness;
    process_arg.cpu_mask = state->curr_args.cpu_mask;
    unsigned result = push_to_queue(state->curr_args.queues[0], process_arg);
    const char *result_str
            = result <= push_error ? PUSH_RESULT_DICT[result] : "unknown code";
    printf("return value: %s\n", result_str);
    return STATUS_OK;
}

static void print_process(process_type *p)
{
    if (p == NULL) {
        printf("(null)\n");
        return;
    }

    printf("======================\n"
           "pid:            %zu\n"
           "remaining time: %u\n"
           "niceness:       %u\n"
           "cpu mask:       0x%04" PRIX16 "\n"
           "======================\n",
            get_pid(p->context),
            p->remaining_time,
            p->niceness,
            p->cpu_mask);
}

static enum status_code get_top_handler(state *state)
{
    process_type *result
            = get_top(state->curr_args.queues[0], state->curr_args.cpu_mask);
    print_process(result);
    return STATUS_OK;
}

static enum status_code pop_top_handler(state *state)
{
    process_type out = { 0 };
    bool result = pop_top(
            state->curr_args.queues[0], state->curr_args.cpu_mask, &out);
    print_bool_result(result);
    if (result) {
        print_process(&out);
    }
    return STATUS_OK;
}

static enum status_code run_top_handler(state *state)
{
    int result = run_top(state->curr_args.queues[0],
            state->curr_args.cpu_mask,
            state->curr_args.time);
    printf("return value: %d\n", result);
    return STATUS_OK;
}

static enum status_code renice_handler(state *state)
{
    process_type *process = &state->processes[state->curr_args.proccess_id];
    bool result = renice(state->curr_args.queues[0],
            process->callback,
            process->context,
            state->curr_args.niceness);
    print_bool_result(result);
    return STATUS_OK;
}

static void show_all_queues(state *state)
{
    for (size_t i = 0; i < state->queue_count; i++) {
        printf("%zu: %s\n", i, state->queues[i].name);
    }
}

static void show_queue(priority_queue *q)
{
    if (q->top == NULL) {
        printf("(empty)\n");
    }
    for (priority_queue_item *item = q->top; item != NULL; item = item->next) {
        print_process(&item->process);
    }
}

static enum status_code show_handler(state *state)
{
    if (state->mandatory_args[0] == NULL) {
        show_all_queues(state);
    } else {
        show_queue(state->curr_args.queues[0]);
    }

    return STATUS_OK;
}

static void _free_user_context(context_wrapper *cw)
{
    if (cw->free_callback != NULL) {
        cw->free_callback(cw->user_context);
    }
}

static void set_process(void *context,
        cb_type callback,
        void (*free_callback)(void *),
        state *state)
{
    process_type *process = &state->processes[state->curr_args.proccess_id];
    context_wrapper *cw = process->context;

    _free_user_context(cw);
    cw->user_context = context;
    cw->free_callback = free_callback;
    process->callback = callback;
}

static enum status_code sum_handler(state *state)
{
    void *context = create_sum_context(
            state->curr_args.numbers_count, &state->curr_args.numbers[0]);
    if (context == NULL) {
        return STATUS_ERROR;
    }
    set_process(context, _sum_callback, free_sum_context, state);
    return STATUS_OK;
}

static enum status_code print_handler(state *state)
{
    void *context = create_print_context(state->curr_args.string);
    if (context == NULL) {
        return STATUS_ERROR;
    }
    set_process(context, _print_callback, free_print_context, state);
    return STATUS_OK;
}

static enum status_code list_handler(state *state);

static enum status_code quit_handler(state *state)
{
    UNUSED(state);
    return STATUS_STOP;
}

#define ARGS(...) (const char *[]) { __VA_ARGS__, NULL }
#define NO_ARGS (const char *[]) { NULL }

#define NEW "new"
#define QUEUE_NAME "queue name"
#define PROCESS_ID "process id"
#define TIME "time"
#define CPU_MASK "cpu mask"
#define NICENESS "niceness"
#define NUMBERS "numbers"
#define STRING "string"

typedef struct command
{
    char *name;
    bool has_variadic_args;
    const char **args;
    enum status_code (*run)(state *);
} command;

static const command COMMANDS[] = {
    {
            .name = "create",
            .args = ARGS(NEW " " QUEUE_NAME),
            .run = create_handler,
    },
    {
            .name = "copy",
            .args = ARGS("dest " QUEUE_NAME, "source " QUEUE_NAME),
            .run = copy_handler,
    },
    {
            .name = "clear",
            .args = ARGS(QUEUE_NAME),
            .run = clear_handler,
    },
    {
            .name = "push",
            .args = ARGS(QUEUE_NAME,
                    PROCESS_ID,
                    "remaining " TIME,
                    NICENESS,
                    CPU_MASK),
            .run = push_handler,
    },
    {
            .name = "get_top",
            .args = ARGS(QUEUE_NAME, CPU_MASK),
            .run = get_top_handler,
    },
    {
            .name = "pop_top",
            .args = ARGS(QUEUE_NAME, CPU_MASK),
            .run = pop_top_handler,
    },
    {
            .name = "run_top",
            .args = ARGS(QUEUE_NAME, CPU_MASK, "run " TIME),
            .run = run_top_handler,
    },
    {
            .name = "renice",
            .args = ARGS(QUEUE_NAME, PROCESS_ID, NICENESS),
            .run = renice_handler,
    },
    {
            .name = "show",
            .args = ARGS(QUEUE_NAME),
            .has_variadic_args = true,
            .run = show_handler,
    },
    {
            .name = "sum",
            .args = ARGS(PROCESS_ID, NUMBERS),
            .has_variadic_args = true,
            .run = sum_handler,
    },
    {
            .name = "print",
            .args = ARGS(PROCESS_ID, STRING),
            .run = print_handler,
    },
    {
            .name = "list",
            .args = NO_ARGS,
            .run = list_handler,
    },
    {
            .name = "quit",
            .args = NO_ARGS,
            .run = quit_handler,
    },
    { .name = NULL },
};

static void print_arguments(const command *c)
{
    for (size_t i = 0; c->args[i] != NULL; i++) {
        putchar(' ');
        if (c->args[i + 1] == NULL && c->has_variadic_args) {
            printf("[%s]", c->args[i]);
        } else {
            printf("<%s>", c->args[i]);
        }
    }
}

static enum status_code list_handler(state *state)
{
    UNUSED(state);
    for (size_t i = 0; COMMANDS[i].name != NULL; i++) {
        const command *c = &COMMANDS[i];
        printf("%-7s", c->name);
        print_arguments(c);
        putchar('\n');
    }
    return STATUS_OK;
}

static const command *find_command(const char *command_name)
{
    for (size_t i = 0; COMMANDS[i].name != NULL; i++) {
        if (strcmp(COMMANDS[i].name, command_name) == 0) {
            return &COMMANDS[i];
        }
    }
    return NULL;
}

static bool parse_queue_name(
        const char *queue_name, const char *man_arg, state *state)
{
    named_queue *nq = find_queue_by_name(queue_name, state);
    if (strstr(man_arg, NEW) != NULL) {
        if (nq != NULL) {
            printf("queue named %s already exists\n", queue_name);
            return false;
        }
    } else if (nq == NULL) {
        print_not_found(queue_name);
        return false;
    }

    size_t *idx = &state->curr_args.queues_count;
    state->curr_args.queues[(*idx)++] = &nq->queue;

    return true;
}

static bool parse_cpu_mask(long number, state *state)
{
    if (number > UINT16_MAX) {
        printf("cpu mask %ld is out of range\n", number);
        return false;
    }
    state->curr_args.cpu_mask = number;
    return true;
}

static bool parse_numbers(const char *num_str, state *state)
{
    int number;
    do {
        if (state->curr_args.numbers_count == SUM_NUMBERS_LIMIT) {
            printf(NUMBERS " limit reached\n");
            return false;
        }
        if (!str_to_int(num_str, &number)) {
            return false;
        }

        state->curr_args.numbers[(state->curr_args.numbers_count)++] = number;
    } while ((num_str = next_word(state)) != NULL);
    return true;
}

static bool parse_process_id(unsigned pid, state *state)
{
    if (pid >= PROCESSES_LIMIT) {
        printf("pid not in range <0; %u>\n", PROCESSES_LIMIT - 1);
        return false;
    }
    state->curr_args.proccess_id = pid;
    return true;
}

static bool parse_argument(char *argument, const char *man_arg, state *state)
{
    if (strstr(man_arg, QUEUE_NAME) != NULL) {
        return parse_queue_name(argument, man_arg, state);
    }

    if (strstr(man_arg, NUMBERS) != NULL) {
        return parse_numbers(argument, state);
    }

    if (strstr(man_arg, STRING) != NULL) {
        state->curr_args.string = argument;
        return true;
    }

    unsigned number;
    if (!str_to_unsigned(argument, &number)) {
        return false;
    }

    if (strstr(man_arg, PROCESS_ID) != NULL) {
        return parse_process_id(number, state);
    } else if (strstr(man_arg, TIME) != NULL) {
        state->curr_args.time = number;
    } else if (strstr(man_arg, NICENESS) != NULL) {
        state->curr_args.niceness = number;
    } else if (strstr(man_arg, CPU_MASK) != NULL) {
        return parse_cpu_mask(number, state);
    } else if (strstr(man_arg, NUMBERS) != NULL) {
        return parse_cpu_mask(number, state);
    }
    return true;
}

static bool parse_mandatory_args(const struct command *command, state *state)
{
    for (size_t i = 0; command->args[i] != NULL; i++) {
        char *argument = next_word(state);
        state->mandatory_args[i] = argument;
        if (argument == NULL) {
            if (command->args[i + 1] == NULL && command->has_variadic_args) {
                return true;
            }
            print_missing(command->args[i]);
            return false;
        }
        if (!parse_argument(argument, command->args[i], state)) {
            return false;
        }
    }
    char *word = next_word(state);
    if (word != NULL) {
        printf("unexpected argument: %s\n", word);
        return false;
    }
    return true;
}

static enum status_code parse_line(char *line, state *state)
{
    const char *command_name = strtok(line, state->word_delims);
    if (command_name == NULL) {
        return STATUS_OK;
    }
    const struct command *command = find_command(command_name);
    if (command == NULL) {
        printf("unknown command: %s \n", command_name);
        return STATUS_OK;
    }

    if (!parse_mandatory_args(command, state)) {
        return STATUS_OK;
    }

    return command->run(state);
}

static void reset_curr_args(state *state)
{
    state->curr_args.cpu_mask = 0;
    state->curr_args.niceness = 0;
    state->curr_args.numbers_count = 0;
    state->curr_args.proccess_id = 0;
    state->curr_args.queues_count = 0;
    state->curr_args.string = NULL;
    state->curr_args.time = 0;
}

static void prompt(void)
{
    printf("> ");
    fflush(stdout);
}

static enum status_code get_line(char line_buff[LINE_BUFFER_LIMIT], FILE *stream)
{
    assert(LINE_BUFFER_LIMIT >= 2);
    line_buff[LINE_BUFFER_LIMIT - 2] = '\n';
    if (fgets(line_buff, LINE_BUFFER_LIMIT, stream) != NULL) {
        if (line_buff[LINE_BUFFER_LIMIT - 2] != '\n') {
            fprintf(stderr, "input is too long\n");
            return STATUS_ERROR;
        }
        return STATUS_OK;
    }

    if (ferror(stream)) {
        fprintf(stderr, "error while reading input\n");
        return STATUS_ERROR;
    }
    return STATUS_STOP;
}

static bool main_loop(state *state)
{
    bool success = false;
    char line[LINE_BUFFER_LIMIT];
    enum status_code read_status;
    prompt();
    while ((read_status = get_line(line, stdin)) == STATUS_OK) {
        enum status_code parse_status;
        if ((parse_status = parse_line(line, state)) == STATUS_ERROR) {
            goto clean;
        }

        if (parse_status == STATUS_STOP) {
            break;
        }
        reset_curr_args(state);
        prompt();
    }

    if (read_status == STATUS_ERROR) {
        goto clean;
    }

    success = true;
clean:
    return success;
}

static void _free_processes_context(size_t n, process_type processes[n])
{
    for (size_t i = 0; i < n; i++) {
        context_wrapper *cw = processes[i].context;
        _free_user_context(cw);
        free(cw);
    }
}

static bool init_processes(process_type *processes)
{
    for (size_t i = 0; i < PROCESSES_LIMIT; i++) {
        if (!reset_process(&processes[i], i)) {
            _free_processes_context(i, processes);
            return false;
        }
    }
    return true;
}

static size_t get_arg_count(const command *c)
{
    size_t n = 0;
    while (c->args[n++] != NULL) {
        // Nop
    }
    return n - 1;
}

static size_t get_max_arg_count(void)
{
    size_t max = 0;
    for (size_t i = 0; COMMANDS[i].name != NULL; i++) {
        size_t arg_count = get_arg_count(&COMMANDS[i]);
        max = arg_count > max ? arg_count : max;
    }
    return max;
}

static bool init_state(state *state)
{
    state->queue_count = 0;
    state->process_count = 0;
    state->word_delims = " \n";
    reset_curr_args(state);

    state->curr_args.numbers
            = calloc(SUM_NUMBERS_LIMIT, sizeof(*state->curr_args.numbers));
    if (state->curr_args.numbers == NULL) {
        goto end;
    }

    state->queues = calloc(QUEUES_LIMIT, sizeof(*state->queues));
    if (state->queues == NULL) {
        goto clean_numbers;
    }

    state->processes = calloc(PROCESSES_LIMIT, sizeof(*state->processes));
    if (state->processes == NULL) {
        goto clean_queue;
    }
    if (!init_processes(state->processes)) {
        goto clean_processes;
    }

    state->mandatory_args
            = calloc(get_max_arg_count(), sizeof(*state->mandatory_args));
    if (state->mandatory_args == NULL) {
        goto clean_processes;
    }

    return true;

clean_processes:
    free(state->processes);
clean_queue:
    free(state->queues);
clean_numbers:
    free(state->curr_args.numbers);
end:
    fprintf(stderr, "the memory is exhausted\n");
    return false;
}

static void clean_named_queue(named_queue *nq)
{
    free(nq->name);
    clear_queue(&nq->queue);
}

static void clean_state(state *state)
{
    free(state->mandatory_args);
    _free_processes_context(PROCESSES_LIMIT, state->processes);
    free(state->processes);
    for (size_t i = 0; i < state->queue_count; i++) {
        clean_named_queue(&state->queues[i]);
    }
    free(state->queues);
    free(state->curr_args.numbers);
}

static int usage(const char *program_name)
{
    fprintf(stderr, "usage: %s\n", program_name);
    return EXIT_FAILURE;
}

int main(int argc, char const *argv[])
{
    if (argc > 1) {
        return usage(argv[0]);
    }
    state state;
    if (!init_state(&state)) {
        return EXIT_FAILURE;
    }

    bool success = main_loop(&state);
    clean_state(&state);
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
