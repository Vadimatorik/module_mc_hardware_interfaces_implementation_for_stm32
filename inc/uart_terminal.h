#pragma once

#ifdef __cplusplus

#include "uart_base.h"

#ifdef HAL_UART_MODULE_ENABLED

namespace mc {

class uart_terminal : public uart_base {
public:
    uart_terminal (const uart_cfg *const cfg, uint32_t cfg_num = 1, uint32_t thread_prio = 2);

public:
    void uart_irq_handler (void);

private:
    static void thread (void *obj);

private:
    static const uint32_t TB_THREAD_SIZE = 200;
    USER_OS_STATIC_STACK_TYPE tb_thread[TB_THREAD_SIZE];
    USER_OS_STATIC_TASK_STRUCT_TYPE ts_thread;

private:
    USER_OS_STATIC_QUEUE q_answer;
    static const uint32_t Q_LEN = 10;
    static const uint32_t Q_ITEM_SIZE = sizeof(char);
    uint8_t qb[Q_LEN * Q_ITEM_SIZE];
    USER_OS_STATIC_QUEUE_STRUCT qs;
};

}

#endif

#endif