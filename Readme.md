# Пакет сетевых модулей

Включает модули: 
* server-tcp
* server-udp
* client-tcp
* io-queue - 
* statistics - сбор статистики 

Документация [doxygen](https://mambaru.github.io/wfc_core/index.html).
Репозитарий на [github.com](https://github.com/mambaru/wfc_core).

## server-tcp

Минимальная конфигурация:
```json
{
  "server-tcp": [
    {
      "name": "server-tcp1",
      "port": "12345"
    }
  ]
}
```
В этой конфигурации сервер открывает порт на localhost, акцептит подключения, но сбрасывает их при попытке отправить ему данные. Для полноценной работы необходимо 
указать цель, которая будет обрабатывать входящий поток данных. Это как правило jsonrpc-сервис прикладного модуля или вспомогательные jsonrpc-модули. В следующем примере 
входящие сообщения отправляются в сыром виде hash-объекту из [демо](https://github.com/mambaru/demod) примера:
```json
{
  "server-tcp": [
    {
      "name": "server-tcp1",
      "target": "hash1",
      "addr": "0.0.0.0",
      "port": "12345"
    }
  ],
  "hash": [
    {
      "name": "hash1"
    }
  ]
}
```
**server-tcp** поддерживает динамическую реконфигурацию, но при реконфигурировании все текущие соединения сбрасываются. В suspend-режиме работает как эхо-сервис. 
Если включена динамическая реконфигурация и изменен только флаг suspend, то изменения вступают в силу только для новых подключений, а текущие продолжают работать. 

По умолчанию архитектура однопоточная (thrеads=0, работает в основном потоке приложения), при увеличении числа потоков работает в режиме акцептор-на-поток, соединение 
работает в том же потоке, где и получило акцепт. При высоких нагрузках рекомендуется выделить хотя-бы один поток (thrеads=1) в этом режиме сервер так же однопоточный, 
но работает в отдельном потоке, и не захламляет заданиями основную очередь приложения, где крутятся таймеры и пр. Если используются очереди, то выделять большое количество 
потоков для сервера не имеет смысла, лучше увеличить число потоков для разбора очередей (см. [workflow](https://github.com/mambaru/wfc_core)). Если время ответа 
прикладной логики стабильно невысокое, то использование очередей может не дать профита, а только увеличит нагрузку на CPU.

Конфигурация:
```
{
  "server-tcp": [
    {
      "name": "server-tcp1",
      /* В отключенном режиме сервер не создается*/
      "enabled": true,
      /* В режиме suspend работает как эхо-сервер */
      "suspend": false,
      /* Приоритет запуска. Укажите минимальный (максимальное значение), если порты необходимо открыть
         после запуска остальных объектов */
      "startup_priority": 0,
      /* Приоритет остановки. Укажите максимальный (минимальное значение), если порты необходимо закрыть 
         в первую очередь */
      "shutdown_priority": 0,

      /* Номера ядер CPU для потоков сервера, например [1,2,3] */
      "cpu": [],
      
      /* Цель - куда передать полученные данные */
      "target": "",
      
      /* По умолчанию использовать разделитель `\r\n` для сообщений. 
         Если false, то настраивается в connection */
      "rn": true,
      
      /* Не закрывать соединение после отправки ответа */
      "keep_alive": true,
      
      /* Число потоков. 0-в основном потоке приложения, 1-в отдельном потоке, N-акцептор на поток*/
      "threads": 0,
      
      /* Адрес */
      "addr": "",
      /* Порт */
      "port": "",
      /* Системная очередь новых соединений */
      "backlog": 1024,
      /* Максимальное количество одновременных подключения. 0 - без ограничения */
      "max_connections": 0,
      
      /* Настройки соединения */
      "connection": {
        /* В режиме direct_mode=true отключается регистрация нового соединения в целевом объекте.
           Работает быстрее, но не все целевые объекты поддерживают такой режим, например очередь 
           c tracking=true () */
        "direct_mode": false,
        
        /* Размер буфера приема сообщений сокета (SOL_SOCKET/SO_RCVBUF). 0 - не изменять */
        "receive_buffer_size": 0,
        
        /* Размер буфера отправки сокета (SOL_SOCKET/SO_SNDBUF). 0 - не изменять */ */
        "send_buffer_size": 0,
        
        "reader": {
          /* Разделитель входящих сообщений. Если задан, то поле rn-игнорируется и используется
             это значение. Если не задан (="") и если rn=true, то это эквивалентно sep="\r\n".
             Если не задан (="") и если rn=false, то без разделения, для двоичных данных. */
          "sep": "",
          /* Рекомендуемый размер приемного буфера (в бинарном режиме фиксированный)*/
          "bufsize": 4096,
          /* Максимальный рекомендуемый размер приемного буфера */
          "maxbuf": 8192,
          /* Минимальный рекомендуемый размер приемного буфера */
          "minbuf": 0,
          /* Максимальный размер входящего сообщения если указан разделитель. 0-без ограничений.*/
          "maxsize": 0,
          /* Удалять разделитель из */
          "trimsep": true
        },
        
        "writer": {
          /* Разделитель исходящих сообщений. Просто добавляет его в конец сообщения.
             Если задан, то поле rn-игнорируется и используется это значение.
             Если не задан (="") и если rn=true, то это эквивалентно sep="\r\n".
          */
          "sep": "",
          /* Размер исходящего буфера. Если сообщение превышает maxbuf, то оно разбивается на 
             несколько буферов размеров bufsize, для оптимизации отправки */
          "bufsize": 8192,
          /* Максимальный размер буфера, до его разбиения */
          "maxbuf": 8192,
          /* Минимальный размер буфера. Если текущий меньше этого значения, то он объединится 
             со следующим*/
          "minbuf": 0,
          /* Максимальный размер исходящего сообщения. Если 0, то предполагается равным maxbuf */
          "maxsize": 0,
          /* Попытка отправить сообщение превышающее _maxbuf с первой попытки, а только потом 
             разбить оставшиеся. Если возможна отправка сообщений более 1МБ, то рекомендуется 
             установить first_as_is=false, чтобы гарантировано разбить большое сообщения, для максимально 
             эффективной записи в сокет */
          "first_as_is": true
        }
      }
    }
  ]
}
```
Значения по умолчанию для размеров буферов **reader** и **writer** подобраны оптимально, для любых размеров сообщений. Но вы можете уменьшить их,
если основной поток сообщений небольшого размера. Увеличивать размеры больше 64К не имеет смысла для любых размеров сообщений.

Когда клиент подключается к серверу, то тот создает новый объект подключения, о чем уведомляется целевой объект (target). После отключения клиента, 
объект уничтожается, о чем так же происходит уведомление. Эти уведомления можно отключить установив direct_mode=true, если целевые объекты их не 
обрабатывают или могут работать без них. Например очереди по этим уведомлениям выкидывают сообщения для клиентов, которые закрыли сообщения. 
Если включить direct_mode=true, то такой возможности уже не будет, и сообщение будет обработано, даже если получатель ответа уже отключился. 
Однако, если отключений клиентов не происходит (как правило по таймауту), то можно немного снизить нагрузку на CPU включив direct_mode.

## server-udp

Для UDP сервера настроек меньше, но оставшиеся совпадают по описанию с TCP, но нужно иметь ввиду, что это датаграмный протокол, и разбиение 
на сообщения происходит только внутри одной датаграмы. 
```json
{
  "server-udp": [
    {
      "name": "server-udp1",
      "enabled": true,
      "suspend": false,
      "startup_priority": 0,
      "shutdown_priority": 0,
      "cpu": [],
      "target": "",
      "rn": true,
      "threads": 0,
      "addr": "",
      "port": "",
      "receive_buffer_size": 0,
      "send_buffer_size": 0,
      "reader": {
        "sep": "",
        "bufsize": 4096,
        "maxbuf": 8192,
        "minbuf": 0,
        "maxsize": 0,
        "trimsep": true
      },
      "writer": {
        "sep": "",
        "bufsize": 8192,
        "maxbuf": 8192,
        "minbuf": 0,
        "maxsize": 0,
        "first_as_is": true
      }
    }
  ]
}
```

Важно! Значения по умолчанию для reader/writer не совсем корректны и необходимо увеличить **bufsize** до максимального возможного 
размера датаграм. При высоких нагрузках рекомендуется также задать **receive_buffer_size**:

```json
"server-udp": [
  {
    "name": "server-udp1",
    "startup_priority":  1000,
    "shutdown_priority": -1000,
    "addr": "0.0.0.0",
    "port": "38000",
    "target": "service1",
    "reader": { "bufsize": 65535},
    "writer": { "bufsize": 65535},
    "receive_buffer_size": 128000000,
    "threads": 16
  }
]
```

## client-tcp1

```
{
  "client-tcp": [
    {
      "name": "client-tcp1",
      "enabled": true,
      "suspend": false,
      "startup_priority": 0,
      "shutdown_priority": 0,
      /* Workflow для таймеров повторного подключения */
      "workflow": "",
      "rn": true,
      /* Завершить приложения при ошибках сети */
      "abort_if_error": true,
      /* Пул клиентов при старте приложения */
      "startup_pool": 1,
      /* Постоянный пул клиентов */
      "primary_pool": 0,
      /* Вторичный пул клиентов */
      "secondary_pool": 0,
      "addr": "",
      "port": "",
      /* Количество подключений на клиент */
      "connect_count": 1,
      /* Потоков на клиент */
      "threads": 0,
      /* Асинхронное подключение */
      "async_connect": false,
      /* Тамаут повторного подключения в миллисекундах */
      "reconnect_timeout_ms": 1000,
      
      /* Настройки аналогично TCP серверу */
      "connection": {
        "receive_buffer_size": 0,
        "send_buffer_size": 0,
        "reader": {
          "sep": "",
          "bufsize": 4096,
          "maxbuf": 8192,
          "minbuf": 0,
          "maxsize": 0,
          "trimsep": true
        },
        "writer": {
          "sep": "",
          "bufsize": 8192,
          "maxbuf": 8192,
          "minbuf": 0,
          "maxsize": 0,
          "first_as_is": true
        }
      }
    }
  ]
}
```

## io-queue

```
{
  "io-queue": [
    {
      "name": "io-queue1",
      "enabled": true,
      "suspend": false,
      "tracking": false,
      "workflow": "",
      "target": "",
      "callback_queue": false,
      "callback_workflow": ""
    }
  ]
}
```

## Настройка для высоких нагрузок

```cpp
┌─────────────┐
│ server-tcp  ├────────────────────────────┐
└─────────────┘                            │ 
┌─────────────┐                            │
│ server-tcp  ├─┐                          │
└─────────────┘ │     ┌────────────────┐   │┌──────────────┐
                ├─────┤ demo-service   ├───┴┤ demo domain  │
┌─────────────┐ │     └────────────────┘    └──────────────┘
│ server-udp  ├─┘
└─────────────┘
```

## Режим проксирования
