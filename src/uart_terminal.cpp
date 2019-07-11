#include "uart_terminal.h"

#ifdef HAL_UART_MODULE_ENABLED

namespace mc {

uart_terminal::uart_terminal (const uart_cfg *const cfg, uint32_t cfg_num, uint32_t thread_prio) :
    uart_base(cfg, cfg_num) {
    USER_OS_STATIC_TASK_CREATE(uart_terminal::thread,
                               "uart_terminal",
                               TB_THREAD_SIZE,
                               this,
                               thread_prio,
                               this->tb_thread,
                               &this->ts_thread);

    this->q_answer = USER_OS_STATIC_QUEUE_CREATE(Q_LEN, Q_ITEM_SIZE, this->qb, &this->qs);
}


void uart_terminal::thread (void *obj) {
    auto *o = reinterpret_cast<uart_terminal*>(obj);

    //Буфер должен выдерживать даже если вся очередь будет в \n.
    uint8_t buf[30] = {0};
    while(true) {
        uint32_t i = 0;
        uint8_t  b_char = 0;
        USER_OS_QUEUE_RECEIVE(o->q_answer, &b_char, portMAX_DELAY);
        if (b_char != '\r') {
            buf[i++] = b_char;
        } else {
            buf[i++] = '\n';
            buf[i++] = '\r';
        }

        for (;i < sizeof(buf); i++) {
            if (USER_OS_QUEUE_RECEIVE(o->q_answer, &b_char, 10) == pdFALSE) {
                break;
            }

            if (b_char != '\r') {
                buf[i++] = b_char;
            } else {
                buf[i++] = '\n';
                buf[i++] = '\r';
            }
        }

        o->tx(buf, i, 100);
    }
}

void uart_terminal::uart_irq_handler (void) {
    if (this->cfg->byte_handler) {
        if (__HAL_UART_GET_FLAG(&this->u, UART_FLAG_RXNE)) {
            uint8_t data = this->u.Instance->DR;
            static BaseType_t xHigherPrioritTaskWoken = pdFALSE;
            USER_OS_QUEUE_SEND_TO_BACK(this->q_answer, &data, &xHigherPrioritTaskWoken);
            this->cfg->byte_handler(data);
        }
    }

    HAL_UART_IRQHandler(&this->u);
}

}

#endif
