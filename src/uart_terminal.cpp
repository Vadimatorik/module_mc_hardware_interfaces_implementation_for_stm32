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


    while(true) {
        uint32_t i = 0;
        uint8_t  b_char = 0;
        USER_OS_QUEUE_RECEIVE(o->q_answer, &b_char, portMAX_DELAY);
        if (b_char != '\r') {
            o->buf_repack_answer[i++] = b_char;
        } else {
            o->buf_repack_answer[i++] = '\n';
            o->buf_repack_answer[i++] = '\r';
            o->tx(o->buf_repack_answer, i, 100);
            i = 0;
        }

        o->cfg->byte_handler(b_char);

        for (;i < sizeof(buf_repack_answer); i++) {
            if (USER_OS_QUEUE_RECEIVE(o->q_answer, &b_char, 10) == pdFALSE) {
                break;
            }

            if (b_char != '\r') {
                o->buf_repack_answer[i++] = b_char;
            } else {
                o->buf_repack_answer[i++] = '\n';
                o->buf_repack_answer[i++] = '\r';
                o->tx(o->buf_repack_answer, i, 100);
                i = 0;
            }

            o->cfg->byte_handler(b_char);
        }

        o->tx(o->buf_repack_answer, i, 100);
    }
}

void uart_terminal::uart_irq_handler (void) {
    static BaseType_t hp = pdFALSE;
    volatile uint8_t sr = this->u.Instance->SR;
    uint8_t data = this->u.Instance->DR;

    if (this->cfg->byte_handler) {
        if ((sr &UART_FLAG_RXNE) == UART_FLAG_RXNE) {
            xQueueSendFromISR(this->q_answer, &data, &hp);
            if (hp == pdTRUE) {
                taskYIELD();
            }
        }
    }

    HAL_UART_IRQHandler(&this->u);
}

}

#endif
