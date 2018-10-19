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

Конфигурация:
```
{
  "server-tcp": [
    {
      "name": "server-tcp1",
      "addr": "0.0.0.0",
      "port": "12345"
    }
  ]
}
```

Конфигурация:
```
{
  "server-tcp": [
    {
      "name": "server-tcp1",
      "target": "",
      "addr": "0.0.0.0",
      "port": "12345",
      
      "connection": {
        "reader": { "sep": "\r\n" },
        "writer": { "sep": "\r\n" }
      }
    }
  ]
}
```


Конфигурация:
```
{
  "server-tcp": [
    {
      "name": "server-tcp1",
      "enabled": true,
      "suspend": false,
      "startup_priority": 0,
      "shutdown_priority": 0,
      "workflow": "",
      "cpu": [],
      "target": "",
      "keep_alive": true,
      "threads": 0,
      "addr": "",
      "port": "",
      "backlog": 1024,
      "max_connections": 0,
      "connection": {
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
        },
        "receive_buffer_size": 0,
        "send_buffer_size": 0,
        "direct_mode": false
      }
    }
  ]
}
```
