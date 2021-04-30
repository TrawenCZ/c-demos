#ifndef CUT_CORE_H
#define CUT_CORE_H

#if defined(CUT) || defined(CUT_MAIN) || defined(DEBUG)
# define CUT_ENABLED
#elif defined(CUT_ENABLED)
# undef CUT_ENABLED
#endif

#if defined(CUT_ENABLED)
#ifndef CUT_ENABLED_H
#define CUT_ENABLED_H

#ifndef CUT_OS_SPECIFIC_H
#define CUT_OS_SPECIFIC_H

#if defined(__linux__)
#ifndef CUT_LINUX_DEFINE_H
#define CUT_LINUX_DEFINE_H

#if defined(__GNUC__) || defined(__clang)
# define CUT_NORETURN __attribute__((noreturn))
# define CUT_CONSTRUCTOR(name) __attribute__((constructor)) static void name()
# define CUT_UNUSED(name) __attribute__((unused)) name
#else
# error "unsupported compiler"
#endif

#if defined(__clang__)
# pragma clang system_header
#elif defined(__GNUC__)
# pragma GCC system_header
#endif

#define _GNU_SOURCE

#endif // CUT_LINUX_DEFINE_H
#elif defined(__APPLE__)
#ifndef CUT_APPLE_DEFINE_H
#define CUT_APPLE_DEFINE_H

#include <sys/types.h>
#include <sys/sysctl.h>

#if defined(__GNUC__) || defined(__clang)
# define CUT_NORETURN __attribute__((noreturn))
# define CUT_CONSTRUCTOR(name) __attribute__((constructor)) static void name()
# define CUT_UNUSED(name) __attribute__((unused)) name
#else
# error "unsupported compiler"
#endif

#if defined(__clang__)
# pragma clang system_header
#elif defined(__GNUC__)
# pragma GCC system_header
#endif

#endif // CUT_APPLE_DEFINE_H
#elif defined(__unix)
#ifndef CUT_UNIX_DEFINE_H
#define CUT_UNIX_DEFINE_H

#if defined(__GNUC__) || defined(__clang)
# define CUT_NORETURN __attribute__((noreturn))
# define CUT_CONSTRUCTOR(name) __attribute__((constructor)) static void name()
# define CUT_UNUSED(name) __attribute__((unused)) name
#else
# error "unsupported compiler"
#endif

#if defined(__clang__)
# pragma clang system_header
#elif defined(__GNUC__)
# pragma GCC system_header
#endif

#define _GNU_SOURCE

#endif // CUT_UNIX_DEFINE_H
#elif defined(_WIN32)
#ifndef CUT_WINDOWS_DEFINE_H
#define CUT_WINDOWS_DEFINE_H

#if defined(__GNUC__) || defined(__clang)

# define CUT_NORETURN __attribute__((noreturn))
# define CUT_CONSTRUCTOR(name) __attribute__((constructor)) static void name()
# define CUT_UNUSED(name) __attribute__((unused)) name

#elif defined(_MSC_VER)

# define CUT_NORETURN __declspec(noreturn)
# pragma section(".CRT$XCU",read)
# define CUT_CONSTRUCTOR(name)                                          \
    static void name(void);                                             \
    __declspec(allocate(".CRT$XCU")) void (* name ## _)(void) = name;   \
    static void name(void)
# define CUT_UNUSED(name) name

#else
# error "unsupported compiler"
#endif

#if defined(__clang__)
# pragma clang system_header
#elif defined(__GNUC__)
# pragma GCC system_header
#endif


#endif // CUT_WINDOWS_DEFINE_H
#else
# error "unsupported platform"
#endif

#endif
#ifndef CUT_PUBLIC_DECLARATIONS_H
#define CUT_PUBLIC_DECLARATIONS_H

#ifndef CUT_DEFINITIONS_H
#define CUT_DEFINITIONS_H

#define CUT_PRIVATE static

#ifdef __cplusplus
# define CUT_NS_BEGIN extern "C" {
# define CUT_NS_END }
#else
# define CUT_NS_BEGIN
# define CUT_NS_END
#endif

#if !defined(CUT_DEFAULT_TIMEOUT)
# define CUT_DEFAULT_TIMEOUT 3
#endif

#if !defined(CUT_NO_FORK)
# define CUT_NO_FORK cut_IsDebugger()
#else
# undef CUT_NO_FORK
# define CUT_NO_FORK 1
#endif

#if !defined(CUT_NO_COLOR)
# define CUT_NO_COLOR !cut_IsTerminalOutput()
#else
# undef CUT_NO_COLOR
# define CUT_NO_COLOR 1
#endif

#if !defined(CUT_MAX_PATH)
# define CUT_MAX_PATH 80
#endif

#if !defined(CUT_FORMAT)
# define CUT_FORMAT "standard"
#endif

#endif

#include <stdio.h>

CUT_NS_BEGIN

#define CUT_MODE_TEXT 0
#define CUT_MODE_BINARY 1

struct cut_Execution {
    int subtest;
    int current;
    int offset;
};

extern const char *cut_needs[1];
typedef void(*cut_Instance)(struct cut_Execution);
typedef void(*cut_Reporter)(const char *, const char *, unsigned);

struct cut_Setup {
    const char *name;
    cut_Instance test;
    const char *file;
    unsigned line;
    int timeout;
    int timeoutDefined;
    int suppress;
    double points;
    const char **needs;
    unsigned long needSize;
};

#define CUT_SETUP_INIT { NULL, NULL, NULL, 0, 0, 0, 0, 0.0, NULL, 0 }

CUT_NORETURN void cut_Stop(const char *text, const char *file, unsigned line);
void cut_Check(const char *text, const char *file, unsigned line);
void cut_Debug(const char *text, const char *file, unsigned line);

void cut_Register(struct cut_Setup *setup);
void cut_RegisterSubtest(int number, const char *name);
void cut_FileCompare(cut_Reporter reporter, FILE *fd, const char *content, int mode, const char *fdStr, const char *file, unsigned line);
void cut_FormatMessage(cut_Reporter reporter, const char *file, unsigned line, const char *fmt, ...);
int cut_Input(const char *content);

CUT_NS_END

#endif

#define ASSERT(e) do {                                                                  \
    if (!(e)) {                                                                         \
        cut_Stop(#e, __FILE__, __LINE__);                                               \
    } } while(0)

#define ASSERT_FILE(f, content)                                                         \
    cut_FileCompare(cut_Stop, f, content, CUT_MODE_TEXT, #f, __FILE__, __LINE__)

#define ASSERT_FILE_BINARY(f, content)                                                  \
    cut_FileCompare(cut_Stop, f, content, CUT_MODE_BINARY, #f, __FILE__, __LINE__)

#define CHECK(e) do {                                                                   \
    if (!(e)) {                                                                         \
        cut_Check(#e, __FILE__, __LINE__);                                              \
    } } while(0)

#define CHECK_FILE(f, content)                                                          \
    cut_FileCompare(cut_Check, f, content, CUT_MODE_TEXT, #f, __FILE__, __LINE__)

#define CHECK_FILE_BINARY(f, content)                                                   \
    cut_FileCompare(cut_Check, f, content, CUT_MODE_BINARY, #f, __FILE__, __LINE__)

#define INPUT(content) do {                                                             \
    if (!cut_Input(content)) {                                                          \
        cut_Stop("cannot set contents as an input file", __FILE__, __LINE__);           \
    } } while(0)

#define TEST_POINTS(n) cut_setup.points = n
#define TEST_TIMEOUT(n) cut_setup.timeout = n, cut_setup.timeoutDefined = 1
#define TEST_SUPPRESS cut_setup.suppress = 1
#define TEST_NEEDS(...) (void)0 ; static const char *cut_needs[] = {"", ## __VA_ARGS__ }; (void)0


#define CUT_TEST_NAME(name) cut_Test_ ## name
#define CUT_REGISTER_NAME(name) cut_Register_ ## name

#define TEST(testName, ...)                                                             \
    void CUT_TEST_NAME(testName) (struct cut_Execution);                                \
    CUT_CONSTRUCTOR(CUT_REGISTER_NAME(testName)) {                                      \
        static struct cut_Setup cut_setup = CUT_SETUP_INIT;                             \
        __VA_ARGS__ ;                                                                   \
        cut_setup.test = CUT_TEST_NAME(testName);                                       \
        cut_setup.name = #testName;                                                     \
        cut_setup.file = __FILE__;                                                      \
        cut_setup.line = 0;                                                             \
        cut_setup.needSize = sizeof(cut_needs)/sizeof(*cut_needs);                      \
        cut_setup.needs = cut_needs;                                                    \
        cut_Register(&cut_setup);                                                       \
    }                                                                                   \
    void CUT_TEST_NAME(testName) (CUT_UNUSED(struct cut_Execution cut_exec))

#define SUBTEST(name)                                                                   \
    cut_exec.offset = cut_exec.subtest;                                                 \
    ++cut_exec.subtest;                                                                 \
    if (!cut_exec.current)                                                              \
        cut_RegisterSubtest(cut_exec.subtest, name);                                    \
    if (cut_exec.subtest == cut_exec.current)

#define REPEATED_SUBTEST(name, count)                                                   \
    ASSERT(0 < count);                                                                  \
    cut_exec.offset = cut_exec.subtest;                                                 \
    cut_exec.subtest += (count);                                                        \
    if (!cut_exec.current)                                                              \
        cut_RegisterSubtest(cut_exec.subtest, name);                                    \
    if (cut_exec.offset < cut_exec.current && cut_exec.current <= cut_exec.subtest)

#define SUBTEST_NO (cut_exec.current - cut_exec.offset)

#define DEBUG_MSG(...) cut_FormatMessage(cut_Debug, __FILE__, __LINE__, __VA_ARGS__)

#endif
# if defined(CUT_MAIN)
#ifndef CUT_MAIN_H
#define CUT_MAIN_H

#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifndef CUT_DECLARATIONS_H
#define CUT_DECLARATIONS_H

#include <stdint.h>
#include <stdio.h>


CUT_NS_BEGIN

struct cut_Info {
    char *message;
    char *file;
    unsigned line;
    struct cut_Info *next;
};

enum cut_MessageType {
    cut_NO_TYPE = 0,
    cut_MESSAGE_SUBTEST,
    cut_MESSAGE_DEBUG,
    cut_MESSAGE_OK,
    cut_MESSAGE_FAIL,
    cut_MESSAGE_EXCEPTION,
    cut_MESSAGE_TIMEOUT,
    cut_MESSAGE_CHECK
};

enum cut_ResultStatus {
    cut_RESULT_UNKNOWN,
    cut_RESULT_OK,
    cut_RESULT_FILTERED_OUT,
    cut_RESULT_SUPPRESSED,
    cut_RESULT_FAILED,
    cut_RESULT_RETURNED_NON_ZERO,
    cut_RESULT_SIGNALLED,
    cut_RESULT_TIMED_OUT,
    cut_RESULT_SKIPPED
};

struct cut_UnitResult {
    char *name;
    int id;
    enum cut_ResultStatus status;
    char *file;
    unsigned line;
    char *statement;
    char *exceptionType;
    char *exceptionMessage;
    int returnCode;
    int signal;
    struct cut_Info *debug;
    struct cut_Info *check;
};

struct cut_UnitTest {
    int id;
    struct cut_Setup *setup;
    int resultSize;
    struct cut_UnitResult *results;
    struct cut_UnitResult *currentResult;
};

struct cut_UnitTestArray {
    int size;
    int capacity;
    struct cut_UnitTest *tests;
};

struct cut_Arguments {
    int help;
    int list;
    unsigned timeout;
    int timeoutDefined;
    int noFork;
    int noColor;
    char *output;
    int testId;
    int subtestId;
    int matchSize;
    char **match;
    const char *selfName;
    int shortPath;
    const char *format;
};

enum cut_ReturnCodes {
    cut_NORMAL_EXIT = 0,
    cut_ERROR_EXIT = 253,
    cut_FATAL_EXIT = 254,
    cut_PANIC = 255
};

enum cut_Colors {
    cut_NO_COLOR = 0,
    cut_YELLOW_COLOR,
    cut_GREEN_COLOR,
    cut_RED_COLOR
};

struct cut_Queue {
    size_t size;
    struct cut_QueueItem *first;
    struct cut_QueueItem *last;
    struct cut_QueueItem *trash;
};

struct cut_QueueItem {
    int testId;
    int *refCount;
    struct cut_QueueItem *next;
    struct cut_Queue depending;
};

struct cut_Shepherd {
    void *data;
    FILE *output;
    int executed;
    int succeeded;
    int suppressed;
    int failed;
    int filteredOut;
    int skipped;

    double points;
    double maxPoints;

    int pipeWrite;
    int pipeRead;

    const struct cut_Arguments *arguments;
    struct cut_Queue *queuedTests;
    void (*unitRunner)(struct cut_Shepherd *, struct cut_UnitTest *test);
    void (*startTest)(struct cut_Shepherd *, const struct cut_UnitTest *test);
    void (*startSubTests)(struct cut_Shepherd *, const struct cut_UnitTest *test);
    void (*startSubTest)(struct cut_Shepherd *, const struct cut_UnitTest *test);
    void (*endSubTest)(struct cut_Shepherd *, const struct cut_UnitTest *test);
    void (*endTest)(struct cut_Shepherd *, const struct cut_UnitTest *test);
    void (*finalize)(struct cut_Shepherd *);
    void (*clear)(struct cut_Shepherd *);
};

struct cut_EnqueuePair {
    int *appliedNeeds;
    struct cut_QueueItem *self;
    int retry;
};


// core:private

#define CUT_DIE(reason) cut_FatalExit(reason, __FILE__, __LINE__)

CUT_NORETURN int cut_FatalExit(const char *reason, const char *file, unsigned line);
CUT_NORETURN int cut_ErrorExit(const char *reason, ...);

CUT_PRIVATE void cut_ClearInfo(struct cut_Info *info);
CUT_PRIVATE void cut_ClearUnitResult(struct cut_UnitResult *result);

CUT_PRIVATE int cut_Help(const struct cut_Arguments *arguments);
CUT_PRIVATE int cut_List();

// arguments

CUT_PRIVATE void cut_ParseArguments(struct cut_Arguments *arguments, int argc, char **argv);

// execution

CUT_PRIVATE void cut_ExceptionBypass(struct cut_UnitTest *test);
CUT_PRIVATE void cut_RunUnitForkless(struct cut_Shepherd *shepherd, struct cut_UnitTest *test);
CUT_PRIVATE void cut_RunUnitTest(struct cut_Shepherd *shepherd, struct cut_UnitTest *test);
CUT_PRIVATE void cut_RunUnitTests(struct cut_Shepherd *shepherd);

CUT_PRIVATE int cut_FilterOutUnit(const struct cut_Arguments *arguments, const struct cut_UnitTest *test);
CUT_PRIVATE void cut_EnqueueTests(struct cut_Shepherd *shepherd);
CUT_PRIVATE int cut_TestComparator(const void *_lhs, const void *_rhs);
CUT_PRIVATE void cut_InitShepherd(struct cut_Shepherd *shepherd, const struct cut_Arguments *arguments,
                                  struct cut_Queue *queue);
CUT_PRIVATE void cut_ClearShepherd(struct cut_Shepherd *shepherd);

CUT_PRIVATE int cut_Runner(const struct cut_Arguments *arguments);

// messages

CUT_PRIVATE void cut_SendOK();
void cut_DebugMessage(const char *file, unsigned line, const char *fmt, ...);
void cut_Stop(const char *text, const char *file, unsigned line);
void cut_Check(const char *text, const char *file, unsigned line);
#ifdef __cplusplus
CUT_PRIVATE void cut_StopException(const char *type, const char *text);
#endif
CUT_PRIVATE void cut_Timedout();
void cut_RegisterSubtest(int number, const char *name);
CUT_PRIVATE void *cut_PipeReader(int pipeRead, struct cut_UnitTest *test);
CUT_PRIVATE int cut_PrepareSubtests(struct cut_UnitTest *test, int number, const char *name);
CUT_PRIVATE int cut_AddInfo(struct cut_Info **info, unsigned line, const char *file, const char *text);
CUT_PRIVATE int cut_SetTestOk(struct cut_UnitResult *result);
CUT_PRIVATE int cut_SetCheckResult(struct cut_UnitResult *result, unsigned line, const char *file, const char *text);
CUT_PRIVATE int cut_SetFailResult(struct cut_UnitResult *result, unsigned line, const char *file, const char *text);
CUT_PRIVATE int cut_SetExceptionResult(struct cut_UnitResult *result, const char *type, const char *text);

// output

CUT_PRIVATE const char *cut_GetStatus(const struct cut_UnitResult *result, enum cut_Colors *color);
CUT_PRIVATE const char *cut_ShortPath(const struct cut_Arguments *arguments, const char *path);
CUT_PRIVATE const char *cut_Signal(int signal);
CUT_PRIVATE const char *cut_ReturnCode(int returnCode);
CUT_PRIVATE void cut_ShepherdNoop(struct cut_Shepherd *shepherd, ...);

// queue

CUT_PRIVATE void cut_InitQueue(struct cut_Queue *queue);
CUT_PRIVATE struct cut_QueueItem *cut_QueuePushTest(struct cut_Queue *queue, int testId);
CUT_PRIVATE void cut_ClearQueueItems(struct cut_QueueItem *current);
CUT_PRIVATE void cut_ClearQueue(struct cut_Queue *queue);
CUT_PRIVATE struct cut_QueueItem *cut_QueuePopTest(struct cut_Queue *queue);
CUT_PRIVATE void cut_QueueMeltTest(struct cut_Queue *queue, struct cut_QueueItem *toMelt);
CUT_PRIVATE int cut_QueueRePushTest(struct cut_Queue *queue, struct cut_QueueItem *toRePush);
CUT_PRIVATE struct cut_QueueItem *cut_QueueAddTest(struct cut_Queue *queue, struct cut_QueueItem *toAdd);

// platform specific functions
CUT_PRIVATE int cut_IsTerminalOutput();
CUT_PRIVATE int cut_IsDebugger();

CUT_PRIVATE void cut_RedirectIO();
CUT_PRIVATE void cut_ResumeIO();

CUT_PRIVATE int cut_ReopenFile(FILE *file);
CUT_PRIVATE void cut_CloseFile(int fd);
CUT_PRIVATE int64_t cut_Read(int fd, char *destination, size_t bytes);
CUT_PRIVATE int64_t cut_Write(int fd, const char *source, size_t bytes);

CUT_PRIVATE int cut_PreRun(const struct cut_Arguments *arguments);
CUT_PRIVATE void cut_RunUnit(struct cut_Shepherd *shepherd, struct cut_UnitTest *test);

CUT_PRIVATE int cut_PrintColorized(FILE *output, enum cut_Colors color, const char *text);

CUT_NS_END

#endif // CUT_DECLARATIONS_H
#ifndef CUT_EXECUTION_H
#define CUT_EXECUTION_H

#ifndef CUT_GLOBALS_H
#define CUT_GLOBALS_H


CUT_NS_BEGIN

#define CUT_MAX_LOCAL_MESSAGE_LENGTH 4096

CUT_PRIVATE const char *cut_emergencyLog = "cut.log";

// avaliable to all forks, readonly after initialization
CUT_PRIVATE struct cut_UnitTestArray cut_unitTests = {0, 0, NULL};

// manipulation either in main instance during no-fork mode
// or in forked instances
CUT_PRIVATE int cut_noFork;
CUT_PRIVATE int cut_pipeWrite = 0;
CUT_PRIVATE int cut_originalStdIn = 0;
CUT_PRIVATE int cut_originalStdOut = 0;
CUT_PRIVATE int cut_originalStdErr = 0;
CUT_PRIVATE FILE *cut_stdout = NULL;
CUT_PRIVATE FILE *cut_stderr = NULL;
CUT_PRIVATE FILE *cut_stdin = NULL;
CUT_PRIVATE jmp_buf cut_executionPoint;
CUT_PRIVATE int cut_localMessageSize = 0;
CUT_PRIVATE char *cut_localMessageCursor = NULL;
CUT_PRIVATE char cut_localMessage[CUT_MAX_LOCAL_MESSAGE_LENGTH];

CUT_NS_END

#endif // CUT_GLOBALS_H
#ifndef CUT_MESSAGES_H
#define CUT_MESSAGES_H

#ifndef CUT_FRAGMENTS_H
#define CUT_FRAGMENTS_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>


CUT_NS_BEGIN

#define CUT_MAX_SLICE_COUNT 255
#define CUT_MAX_SERIALIZED_LENGTH (256*256-1)
#define CUT_MAX_SIGNAL_SAFE_SERIALIZED_LENGTH 512

struct cut_FragmentSlice {
    char *data;
    uint16_t length;
    struct cut_FragmentSlice *next;
};

struct cut_Fragment {
    uint32_t serializedLength;
    uint8_t id;
    uint8_t sliceCount;
    char *serialized;
    struct cut_FragmentSlice *slices;
    struct cut_FragmentSlice *lastSlice;
};

struct cut_FragmentHeader {
    uint32_t length;
    uint8_t id;
    uint8_t sliceCount;
};

typedef union {
    struct {
        uint32_t length;
        uint32_t processed;
    } structured;
    uint64_t raw;
} cut_FragmentReceiveStatus;

CUT_PRIVATE void cut_FragmentInit(struct cut_Fragment *fragments, int id) {
    fragments->id = id;
    fragments->sliceCount = 0;
    fragments->serializedLength = 0;
    fragments->serialized = NULL;
    fragments->slices = NULL;
    fragments->lastSlice = NULL;
}

CUT_PRIVATE void cut_FragmentClear(struct cut_Fragment *fragments) {
    if (!fragments)
        return;
    if (fragments->serialized)
        free(fragments->serialized);
    while (fragments->slices) {
        struct cut_FragmentSlice *current = fragments->slices;
        fragments->slices = fragments->slices->next;
        free(current->data);
        free(current);
    }
}

CUT_PRIVATE void *cut_FragmentReserve(struct cut_Fragment *fragments, size_t length, int *sliceId) {
    if (!fragments)
        return NULL;
    if (fragments->sliceCount == CUT_MAX_SLICE_COUNT)
        return NULL;
    struct cut_FragmentSlice *slice = (struct cut_FragmentSlice *)malloc(sizeof(struct cut_FragmentSlice));
    if (!slice)
        return NULL;
    slice->data = (char *)malloc(length);
    if (!slice->data) {
        free(slice);
        return NULL;
    }
    slice->length = (uint16_t)length;
    slice->next = NULL;
    if (fragments->lastSlice)
        fragments->lastSlice->next = slice;
    else
        fragments->slices = slice;
    fragments->lastSlice = slice;
    if (sliceId)
        *sliceId = fragments->sliceCount;
    ++fragments->sliceCount;
    return slice->data;
}

CUT_PRIVATE void *cut_FragmentAddString(struct cut_Fragment *fragments, const char *str) {
    if (!fragments)
        return NULL;
    size_t length = strlen(str) + 1;
    void *data = cut_FragmentReserve(fragments, length, NULL);
    if (!data)
        return NULL;
    memcpy(data, str, length);
    return data;
}

CUT_PRIVATE char *cut_FragmentGet(struct cut_Fragment *fragments, int sliceId, size_t *length) {
    if (!fragments)
        return NULL;
    if (sliceId >= fragments->sliceCount)
        return NULL;
    struct cut_FragmentSlice *current = fragments->slices;
    for (int id = 0; id < sliceId; ++id) {
        current = current->next;
    }
    if (length)
        *length = current->length;
    return current->data;
}

CUT_PRIVATE uint32_t cut_FragmentCalculateContentOffset(const struct cut_Fragment *fragments) {
    uint32_t offset = sizeof(struct cut_FragmentHeader);
    offset += fragments->sliceCount * sizeof(uint16_t);
    return offset;
}

CUT_PRIVATE uint32_t cut_FragmentCalculateSpace(const struct cut_Fragment *fragments) {
    uint32_t length = cut_FragmentCalculateContentOffset(fragments);
    for (struct cut_FragmentSlice *current = fragments->slices; current; current = current->next) {
        length += current->length;
    }
    return length;
}

CUT_PRIVATE void cut_FragmentSerializeHeader(struct cut_Fragment *fragments) {
    memset(fragments->serialized, 0, fragments->serializedLength);

    struct cut_FragmentHeader *header = (struct cut_FragmentHeader *)fragments->serialized;
    header->length = fragments->serializedLength;
    header->id = fragments->id;
    header->sliceCount = fragments->sliceCount;
}

CUT_PRIVATE void cut_FragmentSerializeSlices(struct cut_Fragment *fragments) {
    uint32_t contentOffset = cut_FragmentCalculateContentOffset(fragments);
    uint16_t *sliceLength = (uint16_t *)(fragments->serialized + sizeof(struct cut_FragmentHeader));

    for (struct cut_FragmentSlice *current = fragments->slices; current; current = current->next) {
        *sliceLength = current->length;
        ++sliceLength;
        memcpy(fragments->serialized + contentOffset, current->data, current->length);
        contentOffset += current->length;
    }
}

CUT_PRIVATE int cut_FragmentSerialize(struct cut_Fragment *fragments) {
    if (!fragments)
        return 0;
    if (fragments->serialized)
        free(fragments->serialized);

    uint32_t length = cut_FragmentCalculateSpace(fragments);
    if (length > CUT_MAX_SERIALIZED_LENGTH)
        return 0;
    fragments->serialized = (char *)malloc(length);
    if (!fragments->serialized)
        return 0;
    fragments->serializedLength = length;

    cut_FragmentSerializeHeader(fragments);
    cut_FragmentSerializeSlices(fragments);
    return 1;
}

CUT_PRIVATE int cut_FragmentSignalSafeSerialize(struct cut_Fragment *fragments) {
    static char buffer[CUT_MAX_SIGNAL_SAFE_SERIALIZED_LENGTH];
    if (!fragments)
        return 0;
    if (fragments->serialized)
        return 0;

    uint32_t length = cut_FragmentCalculateSpace(fragments);
    if (length > CUT_MAX_SIGNAL_SAFE_SERIALIZED_LENGTH)
        return 0;
    fragments->serialized = buffer;
    fragments->serializedLength = length;

    cut_FragmentSerializeHeader(fragments);
    cut_FragmentSerializeSlices(fragments);
    return 1;
}

CUT_PRIVATE int cut_FragmentDeserialize(struct cut_Fragment *fragments) {
    if (!fragments)
        return 0;
    if (!fragments->serialized)
        return 0;
    struct cut_FragmentHeader *header = (struct cut_FragmentHeader *)fragments->serialized;
    fragments->serializedLength = header->length;
    fragments->id = header->id;
    fragments->sliceCount = 0;
    uint16_t *sliceLength = (uint16_t *)(header + 1);
    uint32_t contentOffset = sizeof(struct cut_FragmentHeader)
                           + header->sliceCount * sizeof(uint16_t);
    for (uint16_t slice = 0; slice < header->sliceCount; ++slice, ++sliceLength) {
        void *data = cut_FragmentReserve(fragments, *sliceLength, NULL);
        if (!data)
            return 0;
        memcpy(data, fragments->serialized + contentOffset, *sliceLength);
        contentOffset += *sliceLength;
    }
    return 1;
}

CUT_PRIVATE int64_t cut_FragmentReceiveContinue(cut_FragmentReceiveStatus *status, void *data, int64_t length) {
    if (!status)
        return 0;
    if (!data) {
        status->raw = 0;
        return sizeof(struct cut_FragmentHeader);
    }
    if (length <= 0)
        return length;
    if (!status->structured.length) {
        if (length != sizeof(struct cut_FragmentHeader))
            return 0;
        status->structured.length = ((struct cut_FragmentHeader*)data)->length;
    }
    status->structured.processed += (uint32_t)length;
    return status->structured.length - status->structured.processed;
}

CUT_PRIVATE size_t cut_FragmentReceiveProcessed(cut_FragmentReceiveStatus *status) {
    if (!status)
        return 0;
    return status->structured.processed;
}

CUT_NS_END

#endif

CUT_NS_BEGIN

CUT_PRIVATE int cut_SendMessage(const struct cut_Fragment *message) {
    size_t remaining = message->serializedLength;
    size_t position = 0;

    int64_t r;
    while (remaining && (r = cut_Write(cut_pipeWrite, message->serialized + position, remaining)) > 0) {
        position += (size_t)r;
        remaining -= (size_t)r;
    }
    return r != -1;
}

CUT_PRIVATE int cut_ReadMessage(int pipeRead, struct cut_Fragment *message) {
    cut_FragmentReceiveStatus status;
    memset(&status, 0, sizeof(status));

    message->serialized = NULL;
    message->serializedLength = 0;
    int64_t r = 0;
    int64_t toRead = 0;
    size_t processed = 0;
    while ((toRead = cut_FragmentReceiveContinue(&status, message->serialized, r)) > 0) {
        processed = cut_FragmentReceiveProcessed(&status);

        if (message->serializedLength < processed + toRead) {
            message->serializedLength = (uint32_t)(processed + toRead);
            message->serialized = (char *)realloc(message->serialized, message->serializedLength);
            if (!message->serialized)
                CUT_DIE("cannot allocate memory for reading a message");
        }
        r = cut_Read(pipeRead, message->serialized + processed, (size_t)toRead);
    }
    processed = cut_FragmentReceiveProcessed(&status);
    if (processed < message->serializedLength) {
        memset(message->serialized, 0, message->serializedLength);
    }
    return toRead != -1;
}

CUT_PRIVATE void cut_ResetLocalMessage() {
    cut_localMessageCursor = NULL;
    cut_localMessageSize = 0;
}

CUT_PRIVATE int cut_SendLocalMessage(struct cut_Fragment *message) {
    if (!cut_noFork)
        return cut_SendMessage(message);
    if (message->serializedLength + cut_localMessageSize > CUT_MAX_LOCAL_MESSAGE_LENGTH)
        return 0;
    memcpy(cut_localMessage + cut_localMessageSize, message->serialized, message->serializedLength);
    cut_localMessageSize += message->serializedLength;
    return 1;
}

CUT_PRIVATE void cut_SendOK() {
    struct cut_Fragment message;
    cut_FragmentInit(&message, cut_MESSAGE_OK);
    cut_FragmentSerialize(&message) || CUT_DIE("cannot serialize ok:fragment");
    cut_SendLocalMessage(&message) || CUT_DIE("cannot send ok:message");
    cut_FragmentClear(&message);
}

void cut_CommonMessage(int id, const char *text, const char *file, unsigned line) {
    struct cut_Fragment message;
    cut_FragmentInit(&message, id);
    unsigned *pLine = (unsigned*)cut_FragmentReserve(&message, sizeof(unsigned), NULL);
    if (!pLine)
        CUT_DIE("cannot insert fragment:line");
    *pLine = line;
    cut_FragmentAddString(&message, file) || CUT_DIE("cannot insert fragment:file");
    cut_FragmentAddString(&message, text) || CUT_DIE("cannot insert fragment:text");
    cut_FragmentSerialize(&message) || CUT_DIE("cannot serialize fragment");

    cut_SendLocalMessage(&message) || CUT_DIE("cannot send message");
    cut_FragmentClear(&message);
}

void cut_FormatMessage(cut_Reporter reporter, const char *file, unsigned line, const char *fmt, ...) {
    va_list args1;
    va_start(args1, fmt);
    va_list args2;
    va_copy(args2, args1);
    size_t length = 1 + vsnprintf(NULL, 0, fmt, args1);
    char *buffer;
    (buffer = (char *)malloc(length)) || CUT_DIE("cannot allocate buffer");
    va_end(args1);
    vsnprintf(buffer, length, fmt, args2);
    va_end(args2);

    reporter(buffer, file, line);
    free(buffer);
}

void cut_Stop(const char *text, const char *file, unsigned line) {
    cut_CommonMessage(cut_MESSAGE_FAIL, text, file, line);
    longjmp(cut_executionPoint, 1);
}

void cut_Check(const char *text, const char *file, unsigned line) {
    cut_CommonMessage(cut_MESSAGE_CHECK, text, file, line);
}

void cut_Debug(const char *text, const char *file, unsigned line) {
    cut_CommonMessage(cut_MESSAGE_DEBUG, text, file, line);
}

CUT_PRIVATE void cut_StopException(const char *type, const char *text) {
    struct cut_Fragment message;
    cut_FragmentInit(&message, cut_MESSAGE_EXCEPTION);
    cut_FragmentAddString(&message, type) || CUT_DIE("cannot insert exception:fragment:type");
    cut_FragmentAddString(&message, text) || CUT_DIE("cannot insert exception:fragment:text");
    cut_FragmentSerialize(&message) || CUT_DIE("cannot serialize exception:fragment");

    cut_SendLocalMessage(&message) || CUT_DIE("cannot send exception:message");
    cut_FragmentClear(&message);
}

CUT_PRIVATE void cut_Timedout() {
    struct cut_Fragment message;
    cut_FragmentInit(&message, cut_MESSAGE_TIMEOUT);
    cut_FragmentSignalSafeSerialize(&message) || CUT_DIE("cannot serialize timeout:fragment");
    cut_SendLocalMessage(&message) || CUT_DIE("cannot send timeout:message");
}

void cut_RegisterSubtest(int number, const char *name) {
    struct cut_Fragment message;
    cut_FragmentInit(&message, cut_MESSAGE_SUBTEST);
    int * pNumber = (int *)cut_FragmentReserve(&message, sizeof(int), NULL);
    if (!pNumber)
        CUT_DIE("cannot insert subtest:fragment:number");
    *pNumber = number;
    cut_FragmentAddString(&message, name) || CUT_DIE("cannot insert subtest:fragment:name");
    cut_FragmentSerialize(&message) || CUT_DIE("cannot serialize subtest:fragment");

    cut_SendLocalMessage(&message) || CUT_DIE("cannot send subtest:message");
    cut_FragmentClear(&message);
}

CUT_PRIVATE int cut_ReadLocalMessage(int pipeRead, struct cut_Fragment *message) {
    if (!cut_localMessageSize)
        return cut_ReadMessage(pipeRead, message);
    // first read
    if (!cut_localMessageCursor)
        cut_localMessageCursor = cut_localMessage;

    // nothing to read
    if (cut_localMessageCursor >= cut_localMessage + cut_localMessageSize)
        return 0;

    cut_FragmentReceiveStatus status;
    memset(&status, 0, sizeof(status));
    message->serialized = NULL;
    message->serializedLength = 0;

    int64_t r = 0;
    int64_t toRead = 0;
    while ((toRead = cut_FragmentReceiveContinue(&status, message->serialized, r)) > 0) {
        size_t processed = cut_FragmentReceiveProcessed(&status);

        if (message->serializedLength < processed + toRead) {
            message->serializedLength = (uint32_t)(processed + toRead);
            message->serialized = (char *)realloc(message->serialized, message->serializedLength);
            if (!message->serialized)
                CUT_DIE("cannot allocate memory for reading a message");
        }
        memcpy(message->serialized + processed, cut_localMessageCursor, (size_t)toRead);
        cut_localMessageCursor += toRead;
        r = toRead;
    }
    return toRead != -1;
}

CUT_PRIVATE void *cut_PipeReader(int pipeRead, struct cut_UnitTest *test) {
    int repeat;
    do {
        repeat = 0;
        struct cut_Fragment message;
        cut_FragmentInit(&message, cut_NO_TYPE);
        cut_ReadLocalMessage(pipeRead, &message) || CUT_DIE("cannot read message");
        cut_FragmentDeserialize(&message) || CUT_DIE("cannot deserialize message");

        switch (message.id) {
        case cut_MESSAGE_SUBTEST:
            message.sliceCount == 2 || CUT_DIE("invalid debug:message format");
            cut_PrepareSubtests(
                test,
                *(int *)cut_FragmentGet(&message, 0, NULL),
                cut_FragmentGet(&message, 1, NULL)
            ) || CUT_DIE("cannot set subtest name");
            repeat = 1;
            break;

        case cut_MESSAGE_DEBUG:
            message.sliceCount == 3 || CUT_DIE("invalid debug:message format");
            cut_AddInfo(
                &test->currentResult->debug,
                *(unsigned *)cut_FragmentGet(&message, 0, NULL),
                cut_FragmentGet(&message, 1, NULL),
                cut_FragmentGet(&message, 2, NULL)
            ) || CUT_DIE("cannot add debug");
            repeat = 1;
            break;

        case cut_MESSAGE_OK:
            message.sliceCount == 0 || CUT_DIE("invalid ok:message format");
            cut_SetTestOk(test->currentResult);
            break;

        case cut_MESSAGE_FAIL:
            message.sliceCount == 3 || CUT_DIE("invalid fail:message format");
            cut_SetFailResult(
                test->currentResult,
                *(unsigned *)cut_FragmentGet(&message, 0, NULL),
                cut_FragmentGet(&message, 1, NULL),
                cut_FragmentGet(&message, 2, NULL)
            ) || CUT_DIE("cannot set fail result");
            break;

        case cut_MESSAGE_EXCEPTION:
            message.sliceCount == 2 || CUT_DIE("invalid exception:message format");
            cut_SetExceptionResult(
                test->currentResult,
                cut_FragmentGet(&message, 0, NULL),
                cut_FragmentGet(&message, 1, NULL)
            ) || CUT_DIE("cannot set exception result");
            break;

        case cut_MESSAGE_TIMEOUT:
            test->currentResult->status = cut_RESULT_TIMED_OUT;
            break;

        case cut_MESSAGE_CHECK:
            message.sliceCount == 3 || CUT_DIE("invalid check:message format");
            cut_SetCheckResult(
                test->currentResult,
                *(unsigned *)cut_FragmentGet(&message, 0, NULL),
                cut_FragmentGet(&message, 1, NULL),
                cut_FragmentGet(&message, 2, NULL)
            ) || CUT_DIE("cannot add check");
            repeat = 1;
            break;
        }
        cut_FragmentClear(&message);
    } while (repeat);
    return NULL;
}

CUT_PRIVATE int cut_PrepareSubtests(struct cut_UnitTest *test, int number, const char *name) {
    struct cut_UnitResult *results = (struct cut_UnitResult *) realloc(test->results, (number + 1) * sizeof(struct cut_UnitResult));
    if (!results)
        return 0;

    long offset = test->currentResult - test->results;
    test->results = results;
    test->currentResult = test->results + offset;


    for (int i = test->resultSize; i <= number; ++i) {
        memset(&results[i], 0, sizeof(results[i]));
        results[i].id = i;
        results[i].name = (char *)malloc(strlen(name) + 1);
        if (!results[i].name)
            return 0;
        strcpy(results[i].name, name);
    }
    test->resultSize = number + 1;
    return 1;
}

CUT_PRIVATE int cut_AddInfo(struct cut_Info **info,
                             unsigned line, const char *file, const char *text) {
    struct cut_Info *item = (struct cut_Info *)malloc(sizeof(struct cut_Info));
    if (!item)
        return 0;
    item->file = (char *)malloc(strlen(file) + 1);
    if (!item->file) {
        free(item);
        return 0;
    }
    item->message = (char *)malloc(strlen(text) + 1);
    if (!item->message) {
        free(item->file);
        free(item);
        return 0;
    }
    strcpy(item->file, file);
    strcpy(item->message, text);
    item->line = line;
    item->next = NULL;
    while (*info) {
        info = &(*info)->next;
    }
    *info = item;
    return 1;
}

CUT_PRIVATE int cut_SetTestOk(struct cut_UnitResult *result) {
    if (result->status == cut_RESULT_UNKNOWN)
        result->status = cut_RESULT_OK;
    return 1;
}

CUT_PRIVATE int cut_SetCheckResult(struct cut_UnitResult *result,
                                   unsigned line, const char *file, const char *text) {
    result->status = cut_RESULT_FAILED;
    return cut_AddInfo(&result->check, line, file, text);
}

CUT_PRIVATE int cut_SetFailResult(struct cut_UnitResult *result,
                                  unsigned line, const char *file, const char *text) {
    result->file = (char *)malloc(strlen(file) + 1);
    if (!result->file)
        return 0;
    result->statement = (char *)malloc(strlen(text) + 1);
    if (!result->statement) {
        free(result->file);
        return 0;
    }
    result->line = line;
    strcpy(result->file, file);
    strcpy(result->statement, text);
    result->status = cut_RESULT_FAILED;
    return 1;
}

CUT_PRIVATE int cut_SetExceptionResult(struct cut_UnitResult *result,
                                       const char *type, const char *text) {
    result->exceptionType = (char *)malloc(strlen(type) + 1);
    if (!result->exceptionType)
        return 0;
    result->exceptionMessage = (char *)malloc(strlen(text) + 1);
    if (!result->exceptionMessage) {
        free(result->exceptionType);
        return 0;
    }
    strcpy(result->exceptionType, type);
    strcpy(result->exceptionMessage, text);
    result->status = cut_RESULT_FAILED;
    return 1;
}

CUT_NS_END

#endif // CUT_MESSAGES_H
#ifndef CUT_OUTPUT_JSON_H
#define CUT_OUTPUT_JSON_H


CUT_NS_BEGIN

struct cut_TestAttributes_json {
    int subtests;
    int subtestsFailed;
};

struct cut_OutputData_json {
    struct cut_TestAttributes_json *attributes;
    int testNumber;
    char *buffer;
    size_t bufferSize;
};

CUT_PRIVATE const char *cut_SanitizeStr_json(struct cut_OutputData_json *data, const char *str) {
    size_t quots = 0;
    size_t size = 0;
    const char *c = str;
    for (; *c; ++c) {
        if (*c == '"')
            ++quots;
    }
    size = c - str + 1;

    if (data->bufferSize < size + quots) {
        data->buffer = (char *) realloc(data->buffer, size + quots);
        data->bufferSize = size + quots;
        if (!data->buffer)
            CUT_DIE("cannot allocate memory for json");
    }

    char *buffer = data->buffer;
    do {
        if (*str == '"' || *str == '\\') {
            *buffer = '\\';
            ++buffer;
        }
    } while (*buffer++ = *str++);
    return data->buffer;
}

CUT_PRIVATE void cut_PrintInfo_json(struct cut_Shepherd *shepherd, const struct cut_UnitTest *test) {
    enum cut_Colors color;
    struct cut_OutputData_json *data = (struct cut_OutputData_json *)shepherd->data;
    fprintf(shepherd->output, ", \"status\": \"%s\"", cut_GetStatus(test->currentResult, &color));
    if (test->currentResult->check) {
        fprintf(shepherd->output, ", \"checks\": [");
        int item = 0;
        for (const struct cut_Info *current = test->currentResult->check; current; current = current->next) {
            if (item++)
                fputc(',', shepherd->output);
            fprintf(shepherd->output, "\"%s (%s:%d)\"", cut_SanitizeStr_json(data, current->message),
                    current->file, current->line);
        }
        fprintf(shepherd->output, "]");
    }
    switch (test->currentResult->status) {
    case cut_RESULT_TIMED_OUT:
        fprintf(shepherd->output, ", \"timeout\": %d", test->setup->timeout);
        break;
    case cut_RESULT_SIGNALLED:
        fprintf(shepherd->output, ", \"signal\": \"%s\"", cut_Signal(test->currentResult->signal));
        break;
    case cut_RESULT_RETURNED_NON_ZERO:
        fprintf(shepherd->output, ", \"return\": \"%s\"", cut_ReturnCode(test->currentResult->returnCode));
        break;
    }
    if (test->currentResult->statement && test->currentResult->file && test->currentResult->line) {
        fprintf(shepherd->output, ", \"assert\": \"%s (%s:%d)\"", cut_SanitizeStr_json(data, test->currentResult->statement),
                test->currentResult->file, test->currentResult->line);
    }
    if (test->currentResult->exceptionType && test->currentResult->exceptionMessage) {
        fprintf(shepherd->output, ", \"exception\": \"%s: %s\"", cut_SanitizeStr_json(data, test->currentResult->exceptionType),
                test->currentResult->exceptionMessage);
    }
    if (test->currentResult->debug) {
        fprintf(shepherd->output, ", \"debug\": [");
        int item = 0;
        for (const struct cut_Info *current = test->currentResult->debug; current; current = current->next) {
            if (item++)
                fputc(',', shepherd->output);
            fprintf(shepherd->output, "\"%s (%s:%d)\"", cut_SanitizeStr_json(data, current->message),
                    current->file, current->line);
        }
        fprintf(shepherd->output, "]");
    }
}

CUT_PRIVATE void cut_StartTest_json(struct cut_Shepherd *shepherd, const struct cut_UnitTest *test) {
    struct cut_OutputData_json *data = (struct cut_OutputData_json *)shepherd->data;
    fputc(data->testNumber++ ? ',' : '[', shepherd->output);
    fprintf(shepherd->output, "{\"name\": \"%s\"", test->setup->name);
    fflush(shepherd->output);
}

CUT_PRIVATE void cut_StartSubTests_json(struct cut_Shepherd *shepherd, const struct cut_UnitTest *test) {
    struct cut_OutputData_json *data = (struct cut_OutputData_json *)shepherd->data;
    data->attributes[test->id].subtests = test->resultSize - 1;

    fprintf(shepherd->output, ", \"subtests\": [");
    fflush(shepherd->output);
}

CUT_PRIVATE void cut_EndSubTest_json(struct cut_Shepherd *shepherd, const struct cut_UnitTest *test) {
    // struct cut_OutputData_json *data = (struct cut_OutputData_json *)shepherd->data;
    // if (test->currentResult->status != cut_RESULT_OK)
    //     ++data->attributes[test->id].subtestsFailed;

    // enum cut_Colors color;
    // const char *status = cut_GetStatus(test->currentResult, &color);
    // if (1 < subtest)
    //     fputc(',', shepherd->output);
    // fprintf(shepherd->output, "{\"name\": \"%s\", \"iteration\": %d",
    //         cut_SanitizeStr_json(data, result->name),
    //         result->id);
    // cut_PrintInfo_json(shepherd, testId, result);
    // fputc('}', shepherd->output);
    // fflush(shepherd->output);
}

CUT_PRIVATE void cut_EndTest_json(struct cut_Shepherd *shepherd, const struct cut_UnitTest *test) {
    // struct cut_OutputData_json *data = (struct cut_OutputData_json *)shepherd->data;
    // struct cut_UnitResult dummy;

    // if (data->attributes[testId].subtests) {
    //     memset(&dummy, 0, sizeof(dummy));
    //     dummy.status = data->attributes[testId].subtestsFailed ? cut_RESULT_FAILED : cut_RESULT_OK;
    //     result = &dummy;
    //     fprintf(shepherd->output, "]");
    // }
    // cut_PrintInfo_json(shepherd, testId, result);
    // fprintf(shepherd->output, ", \"points\":%0.2f}", result->status == cut_RESULT_OK
    //         ? cut_unitTests.tests[testId].setup->points
    //         : 0.0);
    // fflush(shepherd->output);
}

CUT_PRIVATE void cut_Finalize_json(struct cut_Shepherd *shepherd) {
    fprintf(shepherd->output, "]");
    fflush(shepherd->output);
}

CUT_PRIVATE void cut_Clear_json(struct cut_Shepherd *shepherd) {
    struct cut_OutputData_json *data = (struct cut_OutputData_json *)shepherd->data;
    free(data->buffer);
    free(data->attributes);
    free(data);
}

CUT_PRIVATE void cut_InitOutput_json(struct cut_Shepherd *shepherd) {

    shepherd->data = malloc(sizeof(struct cut_OutputData_json));
    if (!shepherd->data)
        CUT_DIE("cannot allocate memory for output");

    struct cut_OutputData_json *data = (struct cut_OutputData_json *)shepherd->data;
    
    data->attributes = (struct cut_TestAttributes_json *) malloc(sizeof(struct cut_TestAttributes_json) * cut_unitTests.size);
    if (!data->attributes)
        CUT_DIE("cannot allocate memory for output");
    memset(data->attributes, 0, sizeof(struct cut_TestAttributes_json) * cut_unitTests.size);
    data->testNumber = 0;
    data->buffer = NULL;
    data->bufferSize = 0;

    shepherd->startTest = cut_StartTest_json;
    shepherd->startSubTests = cut_StartSubTests_json;
    shepherd->endSubTest = cut_EndSubTest_json;
    shepherd->endTest = cut_EndTest_json;
    shepherd->finalize = cut_Finalize_json;
    shepherd->clear = cut_Clear_json;
}

CUT_NS_END

#endif
#ifndef CUT_OUTPUT_STD_H
#define CUT_OUTPUT_STD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


CUT_NS_BEGIN

struct cut_TestAttributes_std {
    int subtests;
    int subtestsFailed;
    int repeatedSubtest;
    int base;
};

struct cut_OutputData_std {
    struct cut_TestAttributes_std *attributes;
};


CUT_PRIVATE void cut_StartTest_std(struct cut_Shepherd *shepherd, const struct cut_UnitTest *test) {
    struct cut_OutputData_std *data = (struct cut_OutputData_std *)shepherd->data;

    ++shepherd->executed;

    int base = fprintf(shepherd->output, "[%3i] %s", shepherd->executed, test->setup->name);
    fflush(shepherd->output);
    data->attributes[test->id].base = base;
    data->attributes[test->id].subtests = 0;
    data->attributes[test->id].subtestsFailed = 0;
}

CUT_PRIVATE void cut_StartSubTests_std(struct cut_Shepherd *shepherd, const struct cut_UnitTest *test) {
    struct cut_OutputData_std *data = (struct cut_OutputData_std *)shepherd->data;
    data->attributes[test->id].subtests = test->resultSize - 1;
    data->attributes[test->id].repeatedSubtest = 1;

    for (int i = 2; i < test->resultSize; ++i) {
        if (strcmp(test->results[i - 1].name, test->results[i].name)) {
            data->attributes[test->id].repeatedSubtest = 0;
            break;
        }
    }

    fprintf(shepherd->output, ": %d subtests\n", data->attributes[test->id].subtests);
    fflush(shepherd->output);
}


CUT_PRIVATE int cut_PrintDetailResult(struct cut_Shepherd *shepherd, const char *indent, const struct cut_UnitTest *test) {
    int extended = 0;
    const struct cut_UnitResult *result = test->currentResult;
    for (const struct cut_Info *current = result->check; current; current = current->next) {
        fprintf(shepherd->output, "%scheck '%s' (%s:%d)\n", indent, current->message,
                cut_ShortPath(shepherd->arguments, current->file), current->line);
        extended = 1;
    }
    switch (result->status) {
    case cut_RESULT_TIMED_OUT:
        fprintf(shepherd->output, "%stimed out (%d s)\n", indent, test->setup->timeout);
        extended = 1;
        break;
    case cut_RESULT_SIGNALLED:
        fprintf(shepherd->output, "%ssignal: %s\n", indent, cut_Signal(result->signal));
        extended = 1;
        break;
    case cut_RESULT_RETURNED_NON_ZERO:
        fprintf(shepherd->output, "%sreturn code: %s\n", indent, cut_ReturnCode(result->returnCode));
        extended = 1;
        break;
    }
    if (result->statement && result->file && result->line) {
        fprintf(shepherd->output, "%sassert '%s' (%s:%d)\n", indent,
                result->statement, cut_ShortPath(shepherd->arguments, result->file), result->line);
        extended = 1;
    }
    if (result->exceptionType && result->exceptionMessage) {
        fprintf(shepherd->output, "%sexception %s: %s\n", indent,
                result->exceptionType, result->exceptionMessage);
        extended = 1;
    }
    if (result->debug) {
        fprintf(shepherd->output, "%sdebug messages:\n", indent);
        for (const struct cut_Info *current = result->debug; current; current = current->next) {
            fprintf(shepherd->output, "%s  %s (%s:%d)\n", indent, current->message,
                    cut_ShortPath(shepherd->arguments, current->file), current->line);
        }
        extended = 1;
    }
    return extended;
}

CUT_PRIVATE void cut_EndSubTest_std(struct cut_Shepherd *shepherd, const struct cut_UnitTest *test) {
    static const char *shortIndent = "    ";
    static const char *longIndent = "        ";
    struct cut_OutputData_std *data = (struct cut_OutputData_std *)shepherd->data;
    enum cut_Colors color;
    const char *status = cut_GetStatus(test->currentResult, &color);
    int lastPosition = 80 - 1 - strlen(status);
    if (test->currentResult->status != cut_RESULT_OK)
        ++data->attributes[test->id].subtestsFailed;

    const char *indent = shortIndent;
    if (!data->attributes[test->id].repeatedSubtest || test->currentResult->id <= 1)
        lastPosition -= fprintf(shepherd->output, "%s%s", indent, test->currentResult->name);
    else {
        lastPosition -= fprintf(shepherd->output, "%s", indent);
        int length = strlen(test->currentResult->name);
        for (int i = 0; i < length; ++i)
            putc(' ', shepherd->output);
        lastPosition -= length;
    }
    if (data->attributes[test->id].repeatedSubtest)
        lastPosition -= fprintf(shepherd->output, " #%d", test->currentResult->id);
    indent = longIndent;

    for (int i = 0; i < lastPosition; ++i) {
        putc('.', shepherd->output);
    }
    if (shepherd->arguments->noColor)
        fprintf(shepherd->output, status);
    else
        cut_PrintColorized(shepherd->output, color, status);

    putc('\n', shepherd->output);
    if (cut_PrintDetailResult(shepherd, indent, test))
        putc('\n', shepherd->output);
    fflush(shepherd->output);


}

CUT_PRIVATE void cut_EndSingleTest_std(struct cut_Shepherd *shepherd, const struct cut_UnitTest *test) {
    static const char *indent = "    ";
    struct cut_OutputData_std *data = (struct cut_OutputData_std *)shepherd->data;
    enum cut_Colors color;
    const char *status = cut_GetStatus(test->results, &color);
    int lastPosition = 80 - 1 - strlen(status) - data->attributes[test->id].base;
    int extended = 0;

    for (int i = 0; i < lastPosition; ++i) {
        putc('.', shepherd->output);
    }
    if (shepherd->arguments->noColor)
        fprintf(shepherd->output, status);
    else
        cut_PrintColorized(shepherd->output, color, status);

    putc('\n', shepherd->output);
    if (!data->attributes[test->id].subtests) {
        extended = cut_PrintDetailResult(shepherd, indent, test);
        if (extended)
            putc('\n', shepherd->output);
    }
    fflush(shepherd->output);

    shepherd->maxPoints += test->setup->points;
    switch (test->results->status) {
    case cut_RESULT_OK:
        ++shepherd->succeeded;
        shepherd->points += test->setup->points;
        break;
    case cut_RESULT_SUPPRESSED:
        ++shepherd->suppressed;
        break;
    case cut_RESULT_SKIPPED:
        ++shepherd->skipped;
        break;
    case cut_RESULT_FILTERED_OUT:
        ++shepherd->filteredOut;
        break;
    default:
        ++shepherd->failed;
        break;
    }
}

CUT_PRIVATE void cut_EndSubtests_std(struct cut_Shepherd *shepherd, const struct cut_UnitTest *test) {
    struct cut_OutputData_std *data = (struct cut_OutputData_std *)shepherd->data;

    data->attributes[test->id].base = fprintf(shepherd->output, "[%3i] %s (overall)", shepherd->executed, test->setup->name);
    cut_EndSingleTest_std(shepherd, test);
}

CUT_PRIVATE void cut_EndTest_std(struct cut_Shepherd *shepherd, const struct cut_UnitTest *test) {
    struct cut_OutputData_std *data = (struct cut_OutputData_std *)shepherd->data;

    if (data->attributes[test->id].subtests)
        cut_EndSubtests_std(shepherd, test);
    else
        cut_EndSingleTest_std(shepherd, test);
}

CUT_PRIVATE void cut_Finalize_std(struct cut_Shepherd *shepherd) {
    struct cut_OutputData_std *data = (struct cut_OutputData_std *)shepherd->data;

    fprintf(shepherd->output,
            "\nSummary:\n"
            "  tests:        %3i\n"
            "  succeeded:    %3i\n"
            "  filtered out: %3i\n"
            "  suppressed:   %3i\n"
            "  skipped:      %3i\n"
            "  failed:       %3i\n",
            cut_unitTests.size,
            shepherd->succeeded,
            shepherd->filteredOut,
            shepherd->suppressed,
            shepherd->skipped,
            shepherd->failed);

    if (0.000000001 <= shepherd->maxPoints) {
        fprintf(shepherd->output,
            "\n"
            "Points:         %6.2f\n",
            shepherd->points);
    }
}

CUT_PRIVATE void cut_Clear_std(struct cut_Shepherd *shepherd) {
    struct cut_OutputData_std *data = (struct cut_OutputData_std *)shepherd->data;
    free(data->attributes);
    free(data);
}

CUT_PRIVATE void cut_InitOutput_std(struct cut_Shepherd *shepherd) {

    shepherd->data = malloc(sizeof(struct cut_OutputData_std));
    if (!shepherd->data)
        CUT_DIE("cannot allocate memory for output");

    struct cut_OutputData_std *data = (struct cut_OutputData_std *)shepherd->data;
    
    data->attributes = (struct cut_TestAttributes_std *) malloc(sizeof(struct cut_TestAttributes_std) * cut_unitTests.size);
    if (!data->attributes)
        CUT_DIE("cannot allocate memory for output");
    memset(data->attributes, 0, sizeof(struct cut_TestAttributes_std) * cut_unitTests.size);

    shepherd->startTest = cut_StartTest_std;
    shepherd->startSubTests = cut_StartSubTests_std;
    shepherd->endSubTest = cut_EndSubTest_std;
    shepherd->endTest = cut_EndTest_std;
    shepherd->finalize = cut_Finalize_std;
    shepherd->clear = cut_Clear_std;
}


CUT_NS_END

#endif

#if defined(__cplusplus)

# include <stdexcept>
# include <typeinfo>
# include <string>

CUT_PRIVATE void cut_ExceptionBypass(struct cut_UnitTest *test) {
    cut_RedirectIO();
    if (!setjmp(cut_executionPoint)) {
        try {
            struct cut_Execution exec = {0, test->currentResult->id, 0};
            test->setup->test(exec);
            cut_SendOK();
        } catch (const std::exception &e) {
            std::string name = typeid(e).name();
            cut_StopException(name.c_str(), e.what() ? e.what() : "(no reason)");
        } catch (...) {
            cut_StopException("unknown type", "(empty message)");
        }
    }

    fflush(stdout);
    fflush(stderr);
    cut_ResumeIO();
}

#else

CUT_NS_BEGIN

CUT_PRIVATE void cut_ExceptionBypass(struct cut_UnitTest *test) {
    cut_RedirectIO();
    if (!setjmp(cut_executionPoint)) {
        struct cut_Execution exec = {0, test->currentResult->id, 0};
        test->setup->test(exec);
        cut_SendOK();
    }

    fflush(stdout);
    fflush(stderr);
    cut_ResumeIO();
}

CUT_NS_END

#endif

CUT_NS_BEGIN

CUT_PRIVATE void cut_RunUnitForkless(struct cut_Shepherd *shepherd, struct cut_UnitTest *test) {
    cut_ExceptionBypass(test);
    cut_PipeReader(-1, test);
    cut_ResetLocalMessage();
}

CUT_PRIVATE void cut_RunUnitTest(struct cut_Shepherd *shepherd, struct cut_UnitTest *test) {

    shepherd->unitRunner(shepherd, test);

    if (test->currentResult->status != cut_RESULT_OK || test->resultSize == 1)
        return;
    int start = 1;
    int stop = test->resultSize;
    shepherd->startSubTests(shepherd, test);

    if (0 < shepherd->arguments->subtestId) {
        if (test->resultSize <= shepherd->arguments->subtestId)
            cut_ErrorExit("Invalid argument - requested to run subtest %d, only avaliable %d subtests are for %s test",
                shepherd->arguments->subtestId, test->resultSize, test->setup->name);
        start = shepherd->arguments->subtestId;
        stop = shepherd->arguments->subtestId + 1;
    }

    for (int i = start; i < stop; ++i) {
        test->currentResult = &test->results[i];
        shepherd->startSubTest(shepherd, test);
        shepherd->unitRunner(shepherd, test);
        shepherd->endSubTest(shepherd, test);
    }
}

CUT_PRIVATE void cut_SkipDependingUnitTests(struct cut_Shepherd *shepherd, struct cut_QueueItem *item, enum cut_ResultStatus result) {
    struct cut_QueueItem *cursor = item->depending.first;
    for (; cursor; cursor = cursor->next) {
        cut_unitTests.tests[cursor->testId].results[0].status = result;
    }
}

CUT_PRIVATE void cut_RunUnitTests(struct cut_Shepherd *shepherd) {

    struct cut_QueueItem *item = NULL;
    while ((item = cut_QueuePopTest(shepherd->queuedTests))) {
        struct cut_UnitTest *test = &cut_unitTests.tests[item->testId];

        shepherd->startTest(shepherd, test);

        if (test->results[0].status == cut_RESULT_UNKNOWN) {
            if (test->setup->suppress)
                test->results[0].status = cut_RESULT_SUPPRESSED;
            else if (cut_FilterOutUnit(shepherd->arguments, test))
                test->results[0].status = cut_RESULT_FILTERED_OUT;
            else
                cut_RunUnitTest(shepherd, test);
        }

        for (int s = 1; s < test->resultSize; ++s) {
            if (test->results[s].status != cut_RESULT_OK) {
                test->results[0].status = cut_RESULT_FAILED;
            }
        }

        if (test->results[0].status != cut_RESULT_OK)
            cut_SkipDependingUnitTests(shepherd, item, cut_RESULT_SKIPPED);

        cut_QueueMeltTest(shepherd->queuedTests, item);

        shepherd->endTest(shepherd, test);
    }
}

CUT_PRIVATE int cut_FilterOutUnit(const struct cut_Arguments *arguments, const struct cut_UnitTest *test) {
    if (arguments->testId >= 0)
        return test->id != arguments->testId;
    if (!arguments->matchSize)
        return 0;

    for (int i = 0; i < arguments->matchSize; ++i) {
        if (strstr(test->setup->name, arguments->match[i]))
            return 0;
    }
    return 1;
}

CUT_PRIVATE int cut_TestComparator(const void *_lhs, const void *_rhs) {
    struct cut_UnitTest *lhs = (struct cut_UnitTest *)_lhs;
    struct cut_UnitTest *rhs = (struct cut_UnitTest *)_rhs;

    int result = strcmp(lhs->setup->file, rhs->setup->file);
    if (!result)
        result = lhs->setup->line <= rhs->setup->line ? -1 : 1;
    return result;
}

struct cut_SortedTestItem {
    const char *name;
    int testId;
};

CUT_PRIVATE int cut_TestFinder(const void *_name, const void *_test) {
    const char *name = (const char *)_name;
    const struct cut_SortedTestItem *test = (const struct cut_SortedTestItem *)_test;

    return strcmp(name, test->name);
}

CUT_PRIVATE int cut_SortTestsByName(const void *_lhs, const void *_rhs) {
    const struct cut_SortedTestItem *lhs = (const struct cut_SortedTestItem *) _lhs;
    const struct cut_SortedTestItem *rhs = (const struct cut_SortedTestItem *) _rhs;
    return strcmp(lhs->name, rhs->name);
}

CUT_PRIVATE void cut_EnqueueTests(struct cut_Shepherd *shepherd) {
    int cyclicDependency = 0;
    struct cut_EnqueuePair *tests = (struct cut_EnqueuePair *) malloc(sizeof(struct cut_EnqueuePair) * cut_unitTests.size);
    memset(tests, 0, sizeof(struct cut_EnqueuePair) * cut_unitTests.size);
    struct cut_Queue localQueue;
    cut_InitQueue(&localQueue);

    for (int testId = 0; testId < cut_unitTests.size; ++testId) {
        cut_unitTests.tests[testId].id = testId;
        if (shepherd->arguments->timeoutDefined || !cut_unitTests.tests[testId].setup->timeoutDefined) {
            cut_unitTests.tests[testId].setup->timeout = shepherd->arguments->timeout;
        }

        if (cut_unitTests.tests[testId].setup->needSize == 1) {
            tests[testId].self = cut_QueuePushTest(shepherd->queuedTests, testId);
        }
        else {
            tests[testId].appliedNeeds = (int *) malloc(sizeof(int) * cut_unitTests.tests[testId].setup->needSize);
            memset(tests[testId].appliedNeeds, 0, sizeof(int) * cut_unitTests.tests[testId].setup->needSize);
            cut_QueuePushTest(&localQueue, testId);
        }
    }

    struct cut_SortedTestItem *sortedTests = (struct cut_SortedTestItem *) malloc(sizeof(struct cut_SortedTestItem) * cut_unitTests.size);
    memset(sortedTests, 0, sizeof(struct cut_SortedTestItem) * cut_unitTests.size);

    for (int testId = 0; testId < cut_unitTests.size; ++testId) {
        sortedTests[testId].name = cut_unitTests.tests[testId].setup->name;
        sortedTests[testId].testId = testId;
    }

    qsort(sortedTests, cut_unitTests.size, sizeof(struct cut_SortedTestItem), cut_SortTestsByName);

    struct cut_QueueItem *current = cut_QueuePopTest(&localQueue);
    for (; current; current = cut_QueuePopTest(&localQueue)) {
        int testId = current->testId;
        const struct cut_Setup *setup = cut_unitTests.tests[testId].setup;
        int needSatisfaction = setup->needSize - 1;
        for (size_t n = 1; n < setup->needSize; ++n, --needSatisfaction) {
            if (tests[testId].appliedNeeds[n])
                continue;
            struct cut_SortedTestItem *need = (struct cut_SortedTestItem *) bsearch(
                setup->needs[n], sortedTests, cut_unitTests.size, sizeof(struct cut_SortedTestItem),
                cut_TestFinder);
            if (!need)
                cut_ErrorExit("Test %s depends on %s; such test does not exists, however.", setup->name, setup->needs[n]);
            if (!tests[need->testId].self) {
                cut_QueueRePushTest(&localQueue, current);
                if (cut_unitTests.size < ++tests[testId].retry) {
                    cyclicDependency = 1;
                    goto end;
                }
                break;
            }
            tests[testId].appliedNeeds[n] = need->testId + 1;
        }
        if (needSatisfaction)
            continue;
        for (size_t n = 1; n < setup->needSize; ++n) {
            int needId = tests[testId].appliedNeeds[n] - 1;
            if (!tests[testId].self)
                tests[testId].self = cut_QueuePushTest(&tests[needId].self->depending, testId);
            else
                tests[testId].self = cut_QueueAddTest(&tests[needId].self->depending, tests[testId].self);
        }
    }
end:
    for (int testId = 0; testId < cut_unitTests.size; ++testId)
        free(tests[testId].appliedNeeds);
    free(tests);
    free(sortedTests);
    cut_ClearQueue(&localQueue);

    if (cyclicDependency)
        cut_ErrorExit("There is a cyclic dependency between tests.");
}

CUT_PRIVATE void cut_InitShepherd(struct cut_Shepherd *shepherd, const struct cut_Arguments *arguments, struct cut_Queue *queue) {
    cut_noFork = arguments->noFork;
    shepherd->arguments = arguments;
    shepherd->queuedTests = queue;
    shepherd->executed = 0;
    shepherd->succeeded = 0;
    shepherd->suppressed = 0;
    shepherd->filteredOut = 0;
    shepherd->skipped = 0;
    shepherd->failed = 0;
    shepherd->points = 0;
    shepherd->maxPoints = 0;
    shepherd->startTest = (void (*)(struct cut_Shepherd *, const struct cut_UnitTest *test)) cut_ShepherdNoop;
    shepherd->startSubTests = (void (*)(struct cut_Shepherd *, const struct cut_UnitTest *test)) cut_ShepherdNoop;
    shepherd->startSubTest = (void (*)(struct cut_Shepherd *, const struct cut_UnitTest *test)) cut_ShepherdNoop;
    shepherd->endSubTest = (void (*)(struct cut_Shepherd *, const struct cut_UnitTest *test)) cut_ShepherdNoop;
    shepherd->endTest = (void (*)(struct cut_Shepherd *, const struct cut_UnitTest *test)) cut_ShepherdNoop;
    shepherd->finalize = (void (*)(struct cut_Shepherd *)) cut_ShepherdNoop;
    shepherd->clear = (void (*)(struct cut_Shepherd *)) cut_ShepherdNoop;

    shepherd->unitRunner = arguments->noFork ? cut_RunUnitForkless : cut_RunUnit;

    shepherd->output = stdout;
    if (arguments->output) {
        shepherd->output = fopen(arguments->output, "w");
        if (!shepherd->output)
            cut_ErrorExit("cannot open file %s for writing", arguments->output);
    }

    if (!strcmp(arguments->format, "json"))
        cut_InitOutput_json(shepherd);
    else
        cut_InitOutput_std(shepherd);
}

CUT_PRIVATE void cut_ClearShepherd(struct cut_Shepherd *shepherd) {
    cut_ClearQueue(shepherd->queuedTests);
    shepherd->clear(shepherd);

    if (shepherd->output != stdout)
        fclose(shepherd->output);
    free(shepherd->arguments->match);
    for (int i = 0; i < cut_unitTests.size; ++i) {
        for (int s = 1; s < cut_unitTests.tests[i].resultSize; ++s)
            free(cut_unitTests.tests[i].results[s].name);
        free(cut_unitTests.tests[i].results);
    }
    free(cut_unitTests.tests);
}


CUT_PRIVATE int cut_Runner(const struct cut_Arguments *arguments) {
    int returnValue = 0;
    struct cut_Queue queue;
    struct cut_Shepherd shepherd;

    cut_InitQueue(&queue);
    cut_InitShepherd(&shepherd, arguments, &queue);

    qsort(cut_unitTests.tests, cut_unitTests.size, sizeof(struct cut_UnitTest), cut_TestComparator);
    cut_EnqueueTests(&shepherd);

    if (!cut_PreRun(arguments)) {
        cut_RunUnitTests(&shepherd);

        shepherd.finalize(&shepherd);
        returnValue = shepherd.failed;
    }

    cut_ClearShepherd(&shepherd);
    return returnValue;
}

CUT_NS_END

#endif // CUT_EXECUTION_H
#ifndef CUT_FILE_OPERATIONS_H
#define CUT_FILE_OPERATIONS_H


#include <string.h>
#include <stdlib.h>
#include <unistd.h>

struct cut_FileCompare {
    cut_Reporter reporter;
    const char *expectedContent;
    size_t expectedLength;
    const char *givenContent;
    size_t givenLength;
    const char *fdString;
    const char *file;
    unsigned line;
};

CUT_PRIVATE char *cut_ReadFile(int fd, size_t *sizeOut) {
    enum { CHUNK_SIZE = 512 };
    size_t size = CHUNK_SIZE;
    size_t position = 0;
    char *content = (char *) malloc(size);

    int64_t justRead = 0;
    while (1) {
        justRead = cut_Read(fd, content + position, CHUNK_SIZE);
        if (justRead <= 0)
            break;

        position += justRead;
        size = position + CHUNK_SIZE;
        char *extension = (char *) realloc(content, size);
        if (!extension) {
            free(content);
            return NULL;
        }
        content = extension;
    }

    if (justRead == -1) {
        free(content);
        return NULL;
    }

    *sizeOut = position;
    return content;
}

CUT_PRIVATE char *cut_ReadWholeFile(FILE *file, size_t *sizeOut) {
    if (!file || !sizeOut)
        return NULL;

    fflush(file);
    int fd = cut_ReopenFile(file);
    char *result = cut_ReadFile(fd, sizeOut);

    cut_CloseFile(fd);
    return result;
}


CUT_PRIVATE void cut_FileCompareBinary(const struct cut_FileCompare *pack) {
    if (pack->expectedLength != pack->givenLength) {
        cut_FormatMessage(pack->reporter, pack->file, pack->line,
                          "Lengths of the contents differ; expected: %u, given: %u",
                          pack->expectedLength, pack->givenLength);
        return;
    }

    for (unsigned i = 0; i < pack->expectedLength; ++i) {
        if (pack->expectedContent[i] != pack->givenContent[i]) {
            cut_FormatMessage(pack->reporter, pack->file, pack->line,
                              "Content differs at position %u: expected 0x%X, given 0x%X",
                              i, pack->expectedContent[i], pack->givenContent[i]);
            return;
        }
    }
}

// TODO: implement diff algorithm
CUT_PRIVATE void cut_FileCompareText(const struct cut_FileCompare *pack) {
    cut_FileCompareBinary(pack);
}

void cut_FileCompare(cut_Reporter reporter, FILE *fd, const char *expected, int mode, const char *fdStr, const char *file, unsigned line) {
    struct cut_FileCompare pack = {};
    pack.reporter = reporter;
    pack.expectedContent = expected;
    pack.expectedLength = strlen(expected);
    pack.fdString = fdStr;
    pack.file = file;
    pack.line = line;

    (pack.givenContent = cut_ReadWholeFile(fd, &pack.givenLength)) || CUT_DIE("cannot alocate memory");

    switch (mode) {
    case CUT_MODE_BINARY:
        cut_FileCompareBinary(&pack);
        break;

    case CUT_MODE_TEXT:
        cut_FileCompareText(&pack);
        break;

    default:
        CUT_DIE("unknown mode");
    }

    free((void *)pack.givenContent);
}

int cut_Input(const char *content) {
    size_t remaining = strlen(content);
    size_t offset = 0;

    int64_t r;
    while (remaining && (r = cut_Write(0, content + offset, remaining)) > 0) {
        offset += (size_t)r;
        remaining -= (size_t)r;
    }
    fseek(stdin, 0, SEEK_SET);
    return r != -1;
}

#endif
#ifndef CUT_OUTPUT_H
#define CUT_OUTPUT_H


CUT_NS_BEGIN

CUT_PRIVATE const char *cut_GetStatus(const struct cut_UnitResult *result, enum cut_Colors *color) {
    static const char *unknown = "UNKNOWN";
    static const char *ok = "OK";
    static const char *suppressed = "SUPPRESSED";
    static const char *filteredOut = "FILTERED OUT";
    static const char *returnedNonZero = "NON ZERO";
    static const char *signalled = "SIGNALLED";
    static const char *timedOut = "TIMED OUT";
    static const char *fail = "FAIL";
    static const char *skipped = "SKIPPED";
    static const char *internalFail = "INTERNAL ERROR";

    switch (result->status) {
    case cut_RESULT_OK:
        *color = cut_GREEN_COLOR;
        return ok;
    case cut_RESULT_SUPPRESSED:
        *color = cut_YELLOW_COLOR;
        return suppressed;
    case cut_RESULT_FILTERED_OUT:
        *color = cut_GREEN_COLOR;
        return filteredOut;
    case cut_RESULT_FAILED:
        *color = cut_RED_COLOR;
        return fail;
    case cut_RESULT_RETURNED_NON_ZERO:
        *color = cut_RED_COLOR;
        return returnedNonZero;
    case cut_RESULT_SIGNALLED:
        *color = cut_RED_COLOR;
        return signalled;
    case cut_RESULT_TIMED_OUT:
        *color = cut_RED_COLOR;
        return timedOut;
    case cut_RESULT_SKIPPED:
        *color = cut_YELLOW_COLOR;
        return skipped;
    case cut_RESULT_UNKNOWN:
    default:
        *color = cut_YELLOW_COLOR;
        return unknown;
    }
}

CUT_PRIVATE const char *cut_ShortPath(const struct cut_Arguments *arguments, const char *path) {
    static char shortenedPath[CUT_MAX_PATH + 1];
    char *cursor = shortenedPath;
    const char *dots = "...";
    const size_t dotsLength = strlen(dots);
    int pathLength = strlen(path);
    if (arguments->shortPath < 0 || pathLength <= arguments->shortPath)
        return path;
    
    int fullName = 0;
    const char *end = path + strlen(path);
    const char *name = end;
    for (; end - name < arguments->shortPath && path < name; --name) {
        if (*name == '/' || *name == '\\') {
            fullName = 1;
            break;
        }
    }
    int consumed = (end - name) + dotsLength;
    if (consumed < arguments->shortPath) {
        size_t remaining = arguments->shortPath - consumed;
        size_t firstPart = remaining - remaining / 2;
        strncpy(cursor, path, firstPart);
        cursor += firstPart;
        strcpy(cursor, dots);
        cursor += dotsLength;
        remaining -= firstPart;
        name -= remaining;
    }
    strcpy(cursor, name);
    return shortenedPath;
}

CUT_PRIVATE const char *cut_Signal(int signal) {
    static char number[16];
    const char *names[] = {
        "SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP", "SIGABRT",
        "SIGEMT", "SIGFPE", "SIGKILL", "SIGBUS", "SIGSEGV", "SIGSYS",
        "SIGPIPE", "SIGALRM", "SIGTERM", "SIGUSR1", "SIGUSR2"
    };
    if (0 < signal <= sizeof(names) / sizeof(*names))
        sprintf(number, "%s (%d)", names[signal - 1], signal);
    else
        sprintf(number, "%d", signal);
    return number;
}

CUT_PRIVATE const char *cut_ReturnCode(int returnCode) {
    static char number[16];
    switch (returnCode) {
    case cut_ERROR_EXIT:
    case cut_FATAL_EXIT:
        return "FATAL EXIT";
    case cut_PANIC:
        return "PANIC";
    default:
        sprintf(number, "%d", returnCode);
        return number;
    }
}

CUT_PRIVATE void cut_ShepherdNoop(struct cut_Shepherd *shepherd, ...) {
}

CUT_NS_END

#endif
#ifndef CUT_QUEUE_H
#define CUT_QUEUE_H


CUT_NS_BEGIN

CUT_PRIVATE void cut_InitQueue(struct cut_Queue *queue) {
    queue->size = 0;
    queue->first = NULL;
    queue->last = NULL;
    queue->trash = NULL;
}

CUT_PRIVATE struct cut_QueueItem *cut_QueuePushRefCountedTest(struct cut_Queue *queue, int testId, int *refCount) {
    struct cut_QueueItem *item = (struct cut_QueueItem *) malloc(sizeof(struct cut_QueueItem));
    if (!item)
        CUT_DIE("cannot allocate memory for queue item");

    item->testId = testId;
    item->refCount = refCount;
    item->next = NULL;
    cut_InitQueue(&item->depending);
    if (!queue->first)
        queue->first = item;
    else
        queue->last->next = item;
    queue->last = item;
    ++queue->size;
    return item;
}

CUT_PRIVATE struct cut_QueueItem *cut_QueuePushTest(struct cut_Queue *queue, int testId) {
    struct cut_QueueItem *item = cut_QueuePushRefCountedTest(queue, testId, NULL);
    item->refCount = (int *) malloc(sizeof(int));
    if (!item->refCount)
        CUT_DIE("cannot allocate memory for refCount");
    *item->refCount = 1;
    return item;
}

CUT_PRIVATE void cut_ClearQueueItem(struct cut_QueueItem *toFree) {
    --*toFree->refCount;
    if (!*toFree->refCount)
        free(toFree->refCount);

    cut_ClearQueue(&toFree->depending);
    free(toFree);
}


CUT_PRIVATE void cut_ClearQueueItems(struct cut_QueueItem *current) {
    while (current) {
        struct cut_QueueItem *toFree = current;
        current = current->next;
        cut_ClearQueueItem(toFree);
    };
}


CUT_PRIVATE void cut_ClearQueue(struct cut_Queue *queue) {
    cut_ClearQueueItems(queue->first);
    cut_ClearQueueItems(queue->trash);
    queue->first = NULL;
    queue->last = NULL;
    queue->trash = NULL;
    queue->size = 0;
}


CUT_PRIVATE struct cut_QueueItem *cut_QueuePopTest(struct cut_Queue *queue) {
    if (!queue->size)
        return NULL;
    
    struct cut_QueueItem *item = queue->first;
    queue->first = item->next;
    if (!queue->first)
        queue->last = NULL;
    --queue->size;

    item->next = queue->trash;
    queue->trash = item;

    return item;
}


CUT_PRIVATE void cut_QueueMeltTest(struct cut_Queue *queue, struct cut_QueueItem *toMelt) {
    if (!toMelt->depending.size)
        return;

    struct cut_QueueItem **cursor;

    if (!queue->size) {
        queue->first = toMelt->depending.first;
        cursor = &queue->first;
    }
    else {
        queue->last->next = toMelt->depending.first;
        cursor = &queue->last->next;
    }

    while (*cursor) {
        if (1 < *(*cursor)->refCount) {
            struct cut_QueueItem *toFree = *cursor;
            *cursor = (*cursor)->next;
            cut_ClearQueueItem(toFree);
        }
        else {
            ++queue->size;
            queue->last = *cursor;
            cursor = &(*cursor)->next;
        }
    }
    queue->last->next = NULL;
    toMelt->depending.first = NULL;
    toMelt->depending.last = NULL;
    toMelt->depending.size = 0;
}

CUT_PRIVATE int cut_QueueRePushTest(struct cut_Queue *queue, struct cut_QueueItem *toRePush) {
    struct cut_QueueItem **cursor = &queue->trash;
    int found = 0;
    for (; *cursor; cursor = &(*cursor)->next) {
        if (*cursor == toRePush) {
            found = 1;
            *cursor = (*cursor)->next;
            break;
        }
    }
    if (!found)
        return 0;

    if (!queue->first)
        queue->first = toRePush;
    else
        queue->last->next = toRePush;
    queue->last = toRePush;
    toRePush->next = NULL;
    ++queue->size;
    return 1;
}

CUT_PRIVATE struct cut_QueueItem *cut_QueueAddTest(struct cut_Queue *queue, struct cut_QueueItem *toAdd) {
    struct cut_QueueItem *item = cut_QueuePushRefCountedTest(queue, toAdd->testId, toAdd->refCount);
    ++*item->refCount;
    item->depending.first = toAdd->depending.first;
    item->depending.last = toAdd->depending.last;
    item->depending.trash = toAdd->depending.trash;
    item->depending.size = toAdd->depending.size;
    memset(&toAdd->depending, 0, sizeof(struct cut_Queue));
    return item;
}

CUT_NS_END

#endif

#if defined(__linux__)
#ifndef CUT_LINUX_H
#define CUT_LINUX_H

#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


#ifndef CUT_COMMON_NIX_H
#define CUT_COMMON_NIX_H

#include <unistd.h>


CUT_NS_BEGIN

CUT_PRIVATE int cut_IsTerminalOutput() {
    return isatty(fileno(stdout));
}

CUT_PRIVATE void cut_RedirectIO() {
    cut_stdin = tmpfile();
    cut_stdout = tmpfile();
    cut_stderr = tmpfile();
    cut_originalStdIn = dup(0);
    cut_originalStdOut = dup(1);
    cut_originalStdErr = dup(2);

    dup2(fileno(cut_stdin), 0);
    dup2(fileno(cut_stdout), 1);
    dup2(fileno(cut_stderr), 2);
}

CUT_PRIVATE void cut_ResumeIO() {
    fclose(cut_stdin) != -1 || CUT_DIE("cannot close file");
    fclose(cut_stdout) != -1 || CUT_DIE("cannot close file");
    fclose(cut_stderr) != -1 || CUT_DIE("cannot close file");
    close(0) != -1 || CUT_DIE("cannot close file");
    close(1) != -1 || CUT_DIE("cannot close file");
    close(2) != -1 || CUT_DIE("cannot close file");
    dup2(cut_originalStdIn, 0);
    dup2(cut_originalStdOut, 1);
    dup2(cut_originalStdErr, 2);
}


CUT_PRIVATE int cut_ReopenFile(FILE *file) {
    int fd = dup(fileno(file));
    lseek(fd, 0, SEEK_SET);
    return fd;
}

CUT_PRIVATE void cut_CloseFile(int fd) {
    close(fd);
}

CUT_PRIVATE int64_t cut_Read(int fd, char *destination, size_t bytes) {
    return read(fd, destination, bytes);
}

CUT_PRIVATE int64_t cut_Write(int fd, const char *source, size_t bytes) {
    return write(fd, source, bytes);
}

CUT_PRIVATE int cut_PreRun(const struct cut_Arguments *arguments) {
    return 0;
}

CUT_PRIVATE void cut_SigAlrm(CUT_UNUSED(int signum)) {
    cut_Timedout();
    _exit(cut_NORMAL_EXIT);
}

CUT_PRIVATE int cut_PrintColorized(FILE *output, enum cut_Colors color, const char *text) {
    const char *prefix;
    const char *suffix = "\x1B[0m";
    switch (color) {
    case cut_YELLOW_COLOR:
        prefix = "\x1B[1;33m";
        break;
    case cut_RED_COLOR:
        prefix = "\x1B[1;31m";
        break;
    case cut_GREEN_COLOR:
        prefix = "\x1B[1;32m";
        break;
    default:
        prefix = "";
        suffix = "";
        break;
    }
    return fprintf(output, "%s%s%s", prefix, text, suffix);
}

CUT_NS_END

#endif

CUT_NS_BEGIN

CUT_PRIVATE void cut_RunUnit(struct cut_Shepherd *shepherd, struct cut_UnitTest *test) {
    int r;
    int pipefd[2];
    r = pipe(pipefd);
    if (r == -1)
        CUT_DIE("cannot establish communication pipe");

    int pipeRead = pipefd[0];
    int pipeWrite = pipefd[1];

    int pid = getpid();
    int parentPid = getpid();
    pid = fork();
    if (pid == -1)
        CUT_DIE("cannot fork");
    if (!pid) {
        r = prctl(PR_SET_PDEATHSIG, SIGTERM);
        if (r == -1)
            CUT_DIE("cannot set child death signal");
        if (getppid() != parentPid)
            exit(cut_ERROR_EXIT);
        close(pipeRead) != -1 || CUT_DIE("cannot close file");
        cut_pipeWrite = pipeWrite;

        int timeout = test->setup->timeout;
        if (timeout) {
            signal(SIGALRM, cut_SigAlrm);
            alarm(timeout);
        }
        cut_ExceptionBypass(test);

        close(cut_pipeWrite) != -1 || CUT_DIE("cannot close file");
        cut_ClearShepherd(shepherd);
        exit(cut_NORMAL_EXIT);
    }
    // parent process only
    int status = 0;
    close(pipeWrite) != -1 || CUT_DIE("cannot close file");
    cut_PipeReader(pipeRead, test);
    do {
        r = waitpid(pid, &status, 0);
    } while (r == -1 && errno == EINTR);
    r != -1 || CUT_DIE("cannot wait for unit");
    test->currentResult->returnCode = WIFEXITED(status) ? WEXITSTATUS(status) : 0;
    test->currentResult->signal = WIFSIGNALED(status) ? WTERMSIG(status) : 0;
    if (test->currentResult->signal)
        test->currentResult->status = cut_RESULT_SIGNALLED;
    else if (test->currentResult->returnCode)
        test->currentResult->status = cut_RESULT_RETURNED_NON_ZERO;
    close(pipeRead) != -1 || CUT_DIE("cannot close file");
}

CUT_PRIVATE int cut_IsDebugger() {
    const char *desired = "TracerPid:";
    const size_t desiredLength = strlen(desired);
    const char *found = NULL;
    int tracerPid = 0;
    int result = 0;
    FILE *status = fopen("/proc/self/status", "r");
    if (!status)
        return 0;

    size_t length;
    char *buffer = cut_ReadWholeFile(status, &length);
    fclose(status);

    if (!buffer)
        return 0;

    char *content = (char *) realloc(buffer, length + 1);
    if (content) {
        buffer = content;
        buffer[length] = '\0';
        found = (char *) memmem(buffer, length, desired, desiredLength);
    }

    if (found && desiredLength + 2 <= found - buffer) {
        sscanf(found + desiredLength, "%i", &tracerPid);
    }

    free(buffer);
    return !!tracerPid;
}

CUT_NS_END

#endif // CUT_LINUX_H
#elif defined(__APPLE__)
#ifndef CUT_APPLE_H
#define CUT_APPLE_H

#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#ifndef CUT_COMMON_NIX_H
#define CUT_COMMON_NIX_H

#include <unistd.h>


CUT_NS_BEGIN

CUT_PRIVATE int cut_IsTerminalOutput() {
    return isatty(fileno(stdout));
}

CUT_PRIVATE void cut_RedirectIO() {
    cut_stdin = tmpfile();
    cut_stdout = tmpfile();
    cut_stderr = tmpfile();
    cut_originalStdIn = dup(0);
    cut_originalStdOut = dup(1);
    cut_originalStdErr = dup(2);

    dup2(fileno(cut_stdin), 0);
    dup2(fileno(cut_stdout), 1);
    dup2(fileno(cut_stderr), 2);
}

CUT_PRIVATE void cut_ResumeIO() {
    fclose(cut_stdin) != -1 || CUT_DIE("cannot close file");
    fclose(cut_stdout) != -1 || CUT_DIE("cannot close file");
    fclose(cut_stderr) != -1 || CUT_DIE("cannot close file");
    close(0) != -1 || CUT_DIE("cannot close file");
    close(1) != -1 || CUT_DIE("cannot close file");
    close(2) != -1 || CUT_DIE("cannot close file");
    dup2(cut_originalStdIn, 0);
    dup2(cut_originalStdOut, 1);
    dup2(cut_originalStdErr, 2);
}


CUT_PRIVATE int cut_ReopenFile(FILE *file) {
    int fd = dup(fileno(file));
    lseek(fd, 0, SEEK_SET);
    return fd;
}

CUT_PRIVATE void cut_CloseFile(int fd) {
    close(fd);
}

CUT_PRIVATE int64_t cut_Read(int fd, char *destination, size_t bytes) {
    return read(fd, destination, bytes);
}

CUT_PRIVATE int64_t cut_Write(int fd, const char *source, size_t bytes) {
    return write(fd, source, bytes);
}

CUT_PRIVATE int cut_PreRun(const struct cut_Arguments *arguments) {
    return 0;
}

CUT_PRIVATE void cut_SigAlrm(CUT_UNUSED(int signum)) {
    cut_Timedout();
    _exit(cut_NORMAL_EXIT);
}

CUT_PRIVATE int cut_PrintColorized(FILE *output, enum cut_Colors color, const char *text) {
    const char *prefix;
    const char *suffix = "\x1B[0m";
    switch (color) {
    case cut_YELLOW_COLOR:
        prefix = "\x1B[1;33m";
        break;
    case cut_RED_COLOR:
        prefix = "\x1B[1;31m";
        break;
    case cut_GREEN_COLOR:
        prefix = "\x1B[1;32m";
        break;
    default:
        prefix = "";
        suffix = "";
        break;
    }
    return fprintf(output, "%s%s%s", prefix, text, suffix);
}

CUT_NS_END

#endif

CUT_NS_BEGIN

CUT_PRIVATE void cut_RunUnit(struct cut_Shepherd *shepherd, struct cut_UnitTest *test) {
    int r;
    int pipefd[2];
    r = pipe(pipefd);
    if (r == -1)
        CUT_DIE("cannot establish communication pipe");

    int pipeRead = pipefd[0];
    int pipeWrite = pipefd[1];

    int pid = getpid();
    int parentPid = getpid();
    pid = fork();
    if (pid == -1)
        CUT_DIE("cannot fork");
    if (!pid) {
        /// TODO: missing feature - kill child when parent dies
        close(pipeRead) != -1 || CUT_DIE("cannot close file");
        cut_pipeWrite = pipeWrite;

        int timeout = test->setup->timeout;
        if (timeout) {
            signal(SIGALRM, cut_SigAlrm);
            alarm(timeout);
        }
        cut_ExceptionBypass(test);

        close(cut_pipeWrite) != -1 || CUT_DIE("cannot close file");
        cut_ClearShepherd(shepherd);
        exit(cut_NORMAL_EXIT);
    }
    // parent process only
    int status = 0;
    close(pipeWrite) != -1 || CUT_DIE("cannot close file");
    cut_PipeReader(pipeRead, test);
    do {
        r = waitpid(pid, &status, 0);
    } while (r == -1 && errno == EINTR);
    r != -1 || CUT_DIE("cannot wait for unit");
    test->currentResult->returnCode = WIFEXITED(status) ? WEXITSTATUS(status) : 0;
    test->currentResult->signal = WIFSIGNALED(status) ? WTERMSIG(status) : 0;
    if (test->currentResult->signal)
        test->currentResult->status = cut_RESULT_SIGNALLED;
    else if (test->currentResult->returnCode)
        test->currentResult->status = cut_RESULT_RETURNED_NON_ZERO;
    close(pipeRead) != -1 || CUT_DIE("cannot close file");
}

CUT_PRIVATE int cut_IsDebugger() {
    int                 mib[4];
    struct kinfo_proc   info;
    size_t              size;

    info.kp_proc.p_flag = 0;

    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = getpid();

    size = sizeof(info);
    if (sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0))
        return 0;

    return !!(info.kp_proc.p_flag & P_TRACED);
}

CUT_NS_END

#endif // CUT_APPLE_H
#elif defined(__unix)
#ifndef CUT_UNIX_H
#define CUT_UNIX_H

#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#ifndef CUT_COMMON_NIX_H
#define CUT_COMMON_NIX_H

#include <unistd.h>


CUT_NS_BEGIN

CUT_PRIVATE int cut_IsTerminalOutput() {
    return isatty(fileno(stdout));
}

CUT_PRIVATE void cut_RedirectIO() {
    cut_stdin = tmpfile();
    cut_stdout = tmpfile();
    cut_stderr = tmpfile();
    cut_originalStdIn = dup(0);
    cut_originalStdOut = dup(1);
    cut_originalStdErr = dup(2);

    dup2(fileno(cut_stdin), 0);
    dup2(fileno(cut_stdout), 1);
    dup2(fileno(cut_stderr), 2);
}

CUT_PRIVATE void cut_ResumeIO() {
    fclose(cut_stdin) != -1 || CUT_DIE("cannot close file");
    fclose(cut_stdout) != -1 || CUT_DIE("cannot close file");
    fclose(cut_stderr) != -1 || CUT_DIE("cannot close file");
    close(0) != -1 || CUT_DIE("cannot close file");
    close(1) != -1 || CUT_DIE("cannot close file");
    close(2) != -1 || CUT_DIE("cannot close file");
    dup2(cut_originalStdIn, 0);
    dup2(cut_originalStdOut, 1);
    dup2(cut_originalStdErr, 2);
}


CUT_PRIVATE int cut_ReopenFile(FILE *file) {
    int fd = dup(fileno(file));
    lseek(fd, 0, SEEK_SET);
    return fd;
}

CUT_PRIVATE void cut_CloseFile(int fd) {
    close(fd);
}

CUT_PRIVATE int64_t cut_Read(int fd, char *destination, size_t bytes) {
    return read(fd, destination, bytes);
}

CUT_PRIVATE int64_t cut_Write(int fd, const char *source, size_t bytes) {
    return write(fd, source, bytes);
}

CUT_PRIVATE int cut_PreRun(const struct cut_Arguments *arguments) {
    return 0;
}

CUT_PRIVATE void cut_SigAlrm(CUT_UNUSED(int signum)) {
    cut_Timedout();
    _exit(cut_NORMAL_EXIT);
}

CUT_PRIVATE int cut_PrintColorized(FILE *output, enum cut_Colors color, const char *text) {
    const char *prefix;
    const char *suffix = "\x1B[0m";
    switch (color) {
    case cut_YELLOW_COLOR:
        prefix = "\x1B[1;33m";
        break;
    case cut_RED_COLOR:
        prefix = "\x1B[1;31m";
        break;
    case cut_GREEN_COLOR:
        prefix = "\x1B[1;32m";
        break;
    default:
        prefix = "";
        suffix = "";
        break;
    }
    return fprintf(output, "%s%s%s", prefix, text, suffix);
}

CUT_NS_END

#endif

CUT_NS_BEGIN

CUT_PRIVATE void cut_RunUnit(struct cut_Shepherd *shepherd, struct cut_UnitTest *test) {
    int r;
    int pipefd[2];
    r = pipe(pipefd);
    if (r == -1)
        CUT_DIE("cannot establish communication pipe");

    int pipeRead = pipefd[0];
    int pipeWrite = pipefd[1];

    int pid = getpid();
    int parentPid = getpid();
    pid = fork();
    if (pid == -1)
        CUT_DIE("cannot fork");
    if (!pid) {
        /// TODO: missing feature - kill child when parent dies
        close(pipeRead) != -1 || CUT_DIE("cannot close file");
        cut_pipeWrite = pipeWrite;

        int timeout = test->setup->timeout;
        if (timeout) {
            signal(SIGALRM, cut_SigAlrm);
            alarm(timeout);
        }
        cut_ExceptionBypass(test);

        close(cut_pipeWrite) != -1 || CUT_DIE("cannot close file");
        cut_ClearShepherd(shepherd);
        exit(cut_NORMAL_EXIT);
    }
    // parent process only
    int status = 0;
    close(pipeWrite) != -1 || CUT_DIE("cannot close file");
    cut_PipeReader(pipeRead, test);
    do {
        r = waitpid(pid, &status, 0);
    } while (r == -1 && errno == EINTR);
    r != -1 || CUT_DIE("cannot wait for unit");
    test->currentResult->returnCode = WIFEXITED(status) ? WEXITSTATUS(status) : 0;
    test->currentResult->signal = WIFSIGNALED(status) ? WTERMSIG(status) : 0;
    if (test->currentResult->signal)
        test->currentResult->status = cut_RESULT_SIGNALLED;
    else if (test->currentResult->returnCode)
        test->currentResult->status = cut_RESULT_RETURNED_NON_ZERO;
    close(pipeRead) != -1 || CUT_DIE("cannot close file");
}

CUT_PRIVATE int cut_IsDebugger() {
    const char *desired = "TracerPid:";
    const size_t desiredLength = strlen(desired);
    const char *found = NULL;
    int tracerPid = 0;
    int result = 0;
    FILE *status = fopen("/proc/self/status", "r");
    if (!status)
        return 0;

    size_t length;
    char *buffer = cut_ReadWholeFile(status, &length);
    fclose(status);

    if (!buffer)
        return 0;

    char *content = (char *) realloc(buffer, length + 1);
    if (content) {
        buffer = content;
        buffer[length] = '\0';
        found = (char *) memmem(buffer, length, desired, desiredLength);
    }

    if (found && desiredLength + 2 <= found - buffer) {
        sscanf(found + desiredLength, "%i", &tracerPid);
    }

    free(buffer);
    return !!tracerPid;
}

CUT_NS_END

#endif // CUT_UNIX_H
#elif defined(_WIN32)
#ifndef CUT_WINDOWS_H
#define CUT_WINDOWS_H

#include <Windows.h>
#include <io.h>
#include <fcntl.h>


CUT_NS_BEGIN

CUT_PRIVATE HANDLE cut_jobGroup;

CUT_PRIVATE int cut_IsDebugger() {
    return IsDebuggerPresent();
}

CUT_PRIVATE int cut_IsTerminalOutput() {
    return _isatty(_fileno(stdout));
}

CUT_PRIVATE int cut_CreateTemporaryFile(FILE **file) {
    const char *name = tmpnam(NULL);
    while (*name == '/' || *name == '\\')
        ++name;
    *file = fopen(name, "w+TD");
    return !!*file;
}

CUT_PRIVATE void cut_RedirectIO() {
    cut_CreateTemporaryFile(&cut_stdin) || CUT_DIE("cannot open temporary file");
    cut_CreateTemporaryFile(&cut_stdout) || CUT_DIE("cannot open temporary file");
    cut_CreateTemporaryFile(&cut_stderr) || CUT_DIE("cannot open temporary file");
    cut_originalStdIn = _dup(0);
    cut_originalStdOut = _dup(1);
    cut_originalStdErr = _dup(2);
    _dup2(_fileno(cut_stdin), 0);
    _dup2(_fileno(cut_stdout), 1);
    _dup2(_fileno(cut_stderr), 2);
}

CUT_PRIVATE void cut_ResumeIO() {
    fclose(cut_stdin) != -1 || CUT_DIE("cannot close file");
    fclose(cut_stdout) != -1 || CUT_DIE("cannot close file");
    fclose(cut_stderr) != -1 || CUT_DIE("cannot close file");
    _close(0) != -1 || CUT_DIE("cannot close file");
    _close(1) != -1 || CUT_DIE("cannot close file");
    _close(2) != -1 || CUT_DIE("cannot close file");
    _dup2(cut_originalStdIn, 0);
    _dup2(cut_originalStdOut, 1);
    _dup2(cut_originalStdErr, 2);
}

CUT_PRIVATE int cut_ReopenFile(FILE *file) {
    int fd = _dup(_fileno(file));
    _lseek(fd, 0, SEEK_SET);
    return fd;
}

CUT_PRIVATE void cut_CloseFile(int fd) {
    _close(fd);
}

CUT_PRIVATE int64_t cut_Read(int fd, char *destination, size_t bytes) {
    return _read(fd, destination, bytes);
}

CUT_PRIVATE int64_t cut_Write(int fd, const char *source, size_t bytes) {
    return _write(fd, source, bytes);
}

CUT_PRIVATE void NTAPI cut_TimerCallback(CUT_UNUSED(void *param), CUT_UNUSED(BOOLEAN timerEvent)) {
    cut_Timedout();
    ExitProcess(cut_NORMAL_EXIT);
}

CUT_PRIVATE int cut_PreRun(const struct cut_Arguments *arguments) {
    if (!arguments->noFork && arguments->testId < 0) {
        // create a group of processes to be able to kill unit when parent dies
		cut_jobGroup = CreateJobObject(NULL, NULL);
        if (!cut_jobGroup)
            CUT_DIE("cannot create jobGroup object");
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = {0};
        jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        SetInformationJobObject(cut_jobGroup, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli)) ||CUT_DIE("cannot SetInformationJobObject");
    }
    if (arguments->testId < 0)
        return 0;

    SetErrorMode(SEM_NOGPFAULTERRORBOX);

    HANDLE timer = NULL;
    if (arguments->timeout) {
        CreateTimerQueueTimer(&timer, NULL, cut_TimerCallback, NULL, 
                              arguments->timeout * 1000, 0, WT_EXECUTEONLYONCE) || CUT_DIE("cannot create timer");
    }

    cut_pipeWrite = _dup(1);
    _setmode(cut_pipeWrite, _O_BINARY);

    struct cut_UnitTest *test = &cut_unitTests.tests[arguments->testId];
    test->currentResult->id = arguments->subtestId;
    cut_ExceptionBypass(test);

    _close(cut_pipeWrite) != -1 || CUT_DIE("cannot close file");

    if (timer) {
        DeleteTimerQueueTimer(NULL, timer, NULL) || CUT_DIE("cannot delete timer");
    }

    return 1;
}

CUT_PRIVATE int cut_ErrorCodeToSignal(DWORD returnCode) {
    enum {
        W_SIGHUP = 1,
        W_SIGINT, W_SIGQUIT, W_SIGILL, W_SIGTRAP, W_SIGABRT, W_SIGEMT,
        W_SIGFPE, W_SIGKILL, W_SIGBUS, W_SIGSEGV, W_SIGSYS, W_SIGPIPE,
        W_SIGALRM, W_SIGTERM, W_SIGUSR1, W_SIGUSR2
    };

    if (0 <= (int)returnCode)
        return 0;

    switch (returnCode) {
    case EXCEPTION_ACCESS_VIOLATION:
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
    case EXCEPTION_GUARD_PAGE:
    case EXCEPTION_STACK_OVERFLOW:
        return W_SIGSEGV;

    case EXCEPTION_BREAKPOINT:
    case STATUS_SINGLE_STEP:
        return W_SIGTRAP;

    case EXCEPTION_FLT_DENORMAL_OPERAND:
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    case EXCEPTION_FLT_INEXACT_RESULT:
    case EXCEPTION_FLT_INVALID_OPERATION:
    case EXCEPTION_FLT_OVERFLOW:
    case EXCEPTION_FLT_STACK_CHECK:
    case EXCEPTION_FLT_UNDERFLOW:
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        return W_SIGFPE;

    case EXCEPTION_ILLEGAL_INSTRUCTION:
    case EXCEPTION_PRIV_INSTRUCTION:
        return W_SIGILL;

    case EXCEPTION_IN_PAGE_ERROR:
    case EXCEPTION_INVALID_HANDLE:
        return W_SIGBUS;

    default:
        return returnCode;
    }
}


CUT_PRIVATE void cut_RunUnit(struct cut_Shepherd *shepherd, struct cut_UnitTest *test) {

    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(saAttr);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE childOutWrite, childOutRead;

    CreatePipe(&childOutRead, &childOutWrite, &saAttr, 0);
    SetHandleInformation(childOutRead, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION procInfo;
    STARTUPINFOA startInfo;

    ZeroMemory(&procInfo, sizeof(procInfo));
    ZeroMemory(&startInfo, sizeof(startInfo));

    startInfo.cb = sizeof(startInfo);
    startInfo.dwFlags |= STARTF_USESTDHANDLES;
    startInfo.hStdOutput = childOutWrite;

    const char *fmtString = "\"%s\" --test %i --subtest %i --timeout %i";
    int length = snprintf(NULL, 0, fmtString, shepherd->arguments->selfName,
                          test->id,
                          test->currentResult->id,
                          shepherd->arguments->timeout);
    char *command = (char *)malloc(length + 1);
    sprintf(command, fmtString, shepherd->arguments->selfName,
            test->id,
            test->currentResult->id,
            shepherd->arguments->timeout);
            
    CreateProcessA(shepherd->arguments->selfName,
                   command,
                   NULL,
                   NULL,
                   TRUE,
                   CREATE_BREAKAWAY_FROM_JOB | CREATE_SUSPENDED,
                   NULL,
                   NULL,
                   &startInfo,
                   &procInfo) || CUT_DIE("cannot create process");
    free(command);

    AssignProcessToJobObject(cut_jobGroup, procInfo.hProcess) || CUT_DIE("cannot assign process to job object");
    ResumeThread(procInfo.hThread) == 1 || CUT_DIE("cannot resume thread");
    CloseHandle(childOutWrite) || CUT_DIE("cannot close handle");

    int pipeRead = _open_osfhandle((intptr_t)childOutRead, 0);
    cut_PipeReader(pipeRead, test);

    WaitForSingleObject(procInfo.hProcess, INFINITE) == WAIT_OBJECT_0 || CUT_DIE("cannot wait for single object");
    DWORD childResult;
    GetExitCodeProcess(procInfo.hProcess, &childResult) || CUT_DIE("cannot get exit code");
    CloseHandle(procInfo.hProcess) || CUT_DIE("cannot close handle");
    CloseHandle(procInfo.hThread) || CUT_DIE("cannot close handle");

    test->currentResult->returnCode = 0;
    if ((test->currentResult->signal = cut_ErrorCodeToSignal(childResult))) {
        test->currentResult->status = cut_RESULT_SIGNALLED;
    }
    else {
        test->currentResult->returnCode = childResult;
        test->currentResult->status = childResult ? cut_RESULT_RETURNED_NON_ZERO : cut_RESULT_OK;
    }

    _close(pipeRead) != -1 || CUT_DIE("cannot close file");
}

CUT_PRIVATE int cut_PrintColorized(FILE *output, enum cut_Colors color, const char *text) {
    HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    WORD attributes = 0;

    GetConsoleScreenBufferInfo(stdOut, &info);
    switch (color) {
    case cut_YELLOW_COLOR:
        attributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
        break;
    case cut_RED_COLOR:
        attributes = FOREGROUND_INTENSITY | FOREGROUND_RED;
        break;
    case cut_GREEN_COLOR:
        attributes = FOREGROUND_INTENSITY | FOREGROUND_GREEN;
        break;
    default:
        break;
    }
    if (attributes)
        SetConsoleTextAttribute(stdOut, attributes);
    int rv = fprintf(output, "%s", text);
    if (attributes)
        SetConsoleTextAttribute(stdOut, info.wAttributes);
    return rv;
}

CUT_NS_END

#endif // CUT_WINDOWS_H
#else
# error "unsupported platform"
#endif

CUT_NS_BEGIN

const char *cut_needs[1] = {""};

CUT_NORETURN int cut_FatalExit(const char *reason, const char *file, unsigned line) {
    FILE *log = fopen(cut_emergencyLog, "a");
    if (!log)
        exit(cut_PANIC);

    time_t now = time(NULL);
    struct tm *timeInfo = localtime(&now);

    char when[80];
    strftime(when, sizeof(when), "%F %T", timeInfo);
    fprintf(log, "%s | %s (%u) | %s\n", when, file, line, reason);

    fclose(log);
    exit(cut_FATAL_EXIT);
}

CUT_NORETURN int cut_ErrorExit(const char *reason, ...) {
    va_list args;
    va_start(args, reason);
    fprintf(stderr, "Error happened: ");
    vfprintf(stderr, reason, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(cut_ERROR_EXIT);
}

void cut_Register(struct cut_Setup *setup) {
    if (cut_unitTests.size == cut_unitTests.capacity) {
        cut_unitTests.capacity += 16;
        cut_unitTests.tests = (struct cut_UnitTest *)realloc(cut_unitTests.tests,
            sizeof(struct cut_UnitTest) * cut_unitTests.capacity);
        if (!cut_unitTests.tests)
            CUT_DIE("cannot allocate memory for unit tests");
    }
    struct cut_UnitTest *test = &cut_unitTests.tests[cut_unitTests.size];
    test->id = 0;
    test->setup = setup;
    test->results = (struct cut_UnitResult *) malloc(sizeof(struct cut_UnitResult));
    if (!test->results)
        CUT_DIE("cannot allocate memory for unit tests");

    test->resultSize = 1;
    test->currentResult = test->results;
    memset(test->currentResult, 0, sizeof(*test->currentResult));

    ++cut_unitTests.size;
}

CUT_PRIVATE void cut_ParseArguments(struct cut_Arguments *arguments, int argc, char **argv) {
    static const char *help = "--help";
    static const char *list = "--list";
    static const char *timeout = "--timeout";
    static const char *noFork = "--no-fork";
    static const char *doFork = "--fork";
    static const char *noColor = "--no-color";
    static const char *output = "--output";
    static const char *subtest = "--subtest";
    static const char *exactTest = "--test";
    static const char *shortPath = "--short-path";
    static const char *format = "--format";
    arguments->help = 0;
    arguments->list = 0;
    arguments->timeout = CUT_DEFAULT_TIMEOUT;
    arguments->timeoutDefined = 0;
    arguments->noFork = CUT_NO_FORK;
    arguments->noColor = CUT_NO_COLOR;
    arguments->output = NULL;
    arguments->matchSize = 0;
    arguments->match = NULL;
    arguments->testId = -1;
    arguments->subtestId = -1;
    arguments->selfName = argv[0];
    arguments->shortPath = -1;
    arguments->format = CUT_FORMAT;

    for (int i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "--", 2)) {
            ++arguments->matchSize;
            continue;
        }
        if (!strcmp(help, argv[i])) {
            arguments->help = 1;
            continue;
        }
        if (!strcmp(list, argv[i])) {
            arguments->list = 1;
            continue;
        }
        if (!strcmp(timeout, argv[i])) {
            ++i;
            if (i >= argc || !sscanf(argv[i], "%u", &arguments->timeout))
                cut_ErrorExit("option %s requires numeric argument", timeout);
            arguments->timeoutDefined = 1;
            continue;
        }
        if (!strcmp(noFork, argv[i])) {
            arguments->noFork = 1;
            continue;
        }
        if (!strcmp(doFork, argv[i])) {
            arguments->noFork = 0;
            continue;
        }
        if (!strcmp(noColor, argv[i])) {
            arguments->noColor = 1;
            continue;
        }
        if (!strcmp(output, argv[i])) {
            ++i;
            if (i < argc) {
                arguments->output = argv[i];
                arguments->noColor = 1;
            }
            else {
                cut_ErrorExit("option %s requires string argument", output);
            }
            continue;
        }
        if (!strcmp(exactTest, argv[i])) {
            ++i;
            if (i >= argc || !sscanf(argv[i], "%d", &arguments->testId))
                cut_ErrorExit("option %s requires numeric argument %d %d", exactTest, i, argc);
            continue;
        }
        if (!strcmp(subtest, argv[i])) {
            ++i;
            if (i >= argc || !sscanf(argv[i], "%d", &arguments->subtestId))
                cut_ErrorExit("option %s requires numeric argument", subtest);
            continue;
        }
        if (!strcmp(shortPath, argv[i])) {
            ++i;
            if (i >= argc || !sscanf(argv[i], "%d", &arguments->shortPath))
                cut_ErrorExit("option %s requires numeric argument", shortPath);
            if (arguments->shortPath > CUT_MAX_PATH)
                arguments->shortPath = CUT_MAX_PATH;
            continue;
        }
        if (!strcmp(format, argv[i])) {
            ++i;
            if (i < argc)
                arguments->format = argv[i];
            else
                cut_ErrorExit("option %s requires string argument", format);
            continue;
        }
        cut_ErrorExit("option %s is not recognized", argv[i]);
    }
    if (!arguments->matchSize)
        return;
    arguments->match = (char **)malloc(arguments->matchSize * sizeof(char *));
    if (!arguments->match)
        cut_ErrorExit("cannot allocate memory for list of selected tests");
    int index = 0;
    for (int i = 1; i < argc; ++i) {
        if (!strncmp(argv[i], "--", 2)) {
            if (!strcmp(timeout, argv[i]) || !strcmp(output, argv[i])
             || !strcmp(subtest, argv[i]) || !strcmp(exactTest, argv[i])
             || !strcmp(shortPath, argv[i]))
            {
                ++i;
            }
            continue;
        }
        arguments->match[index++] = argv[i];
    }
}

CUT_PRIVATE void cut_ClearInfo(struct cut_Info *info) {
    while (info) {
        struct cut_Info *current = info;
        info = info->next;
        free(current->message);
        free(current->file);
        free(current);
    }
}

CUT_PRIVATE void cut_ClearUnitResult(struct cut_UnitResult *result) {
    free(result->name);
    free(result->file);
    free(result->statement);
    free(result->exceptionType);
    free(result->exceptionMessage);
    cut_ClearInfo(result->debug);
    cut_ClearInfo(result->check);
}

CUT_PRIVATE int cut_Help(const struct cut_Arguments *arguments) {
    const char *text = ""
    "Run as %s [options] [test names]\n"
    "\n"
    "Options:\n"
    "\t--help            Print out this help.\n"
    "\t--list            Print list of all tests.\n"
    "\t--format <type>   Use different kinds of output formats:\n"
    "\t         json     use json format\n"
    "\t         <?>      use standard terminal friendly format\n"
    "\t--timeout <N>     Set timeout of each test in seconds. 0 for no timeout.\n"
    "\t--no-fork         Disable forking. Timeout is turned off.\n"
    "\t--fork            Force forking. Usefull during debugging with fork enabled.\n"
    "\t--no-color        Turn off colors.\n"
    "\t--output <file>   Redirect output to the file.\n"
    "\t--short-path <N>  Make filenames in the output shorter.\n"
    "Hidden options (for internal purposes only):\n"
    "\t--test <N>        Run test of index N.\n"
    "\t--subtest <N>     Run subtest of index N (for all tests).\n"
    "\n"
    "Test names - any other parameter is accepted as a filter of test names. "
    "In case there is at least one filter parameter, a test is executed only if "
    "at least one of the filters is a substring of the test name."
    "";

    fprintf(stderr, text, arguments->selfName);
    return 0;
}

CUT_PRIVATE int cut_List() {
    fprintf(stderr, "Tests:\n");
    for (int i = 0; i < cut_unitTests.size; ++i) {
        struct cut_UnitTest *test = &cut_unitTests.tests[i];
        fprintf(stderr, "\t%s\n", test->setup->name);
    }
    return 0;
}

int main(int argc, char **argv) {

    struct cut_Arguments arguments;

    cut_ParseArguments(&arguments, argc, argv);

    if (arguments.help)
        return cut_Help(&arguments);
    if (arguments.list)
        return cut_List();

    return cut_Runner(&arguments);
}

CUT_NS_END

#endif
# endif
#else
#ifndef CUT_DISABLED_H
#define CUT_DISABLED_H

#define ASSERT(e) (void)0
#define ASSERT_FILE(f, content) (void)0
#define ASSERT_FILE_BINARY(f, content) (void)0
#define CHECK(e) (void)0
#define CHECK_FILE(f, content) (void)0
#define CHECK_FILE_BINARY(f, content) (void)0
#define INPUT(content) (void)0
#define TEST(name, ...) static void unitTest_ ## name()
#define SUBTEST(name) if (0)
#define REPEATED_SUBTEST(name, count) if (0)
#define SUBTEST_NO 0
#define DEBUG_MSG(...) (void)0
#define TEST_POINTS(n)
#define TEST_TIMEOUT(n)
#define TEST_SUPPRESS
#define TEST_NEEDS(...)

#endif
#endif

#endif // CUT_CORE_H
