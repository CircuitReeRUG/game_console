# game_console
name tbd

## Firmware Block Diagram

```mermaid
flowchart TD
    SD[(SD Card)]

    subgraph System ["Core System"]
        Loader["Loader<br/>(@saraanabota, @KrisPuusepp)<br/>- Polls SD card periodically<br/>- loads new game images"]
        PeriphAPI["Periph API<br/>(@confestim, @MrDragonMister)<br/>- End-user interface<br/>- Manages OLED & Buttons"]
    end

    subgraph UserSpace ["User Programs"]
        Snake["Snake game (Example)<br/>(@justtryingthingsout, @PatrickGM1)"]
        OtherGames["User game"]
    end

    subgraph Hardware ["Peripherals"]
        OLED[OLED Display]
        BTN[Buttons]
    end

    SD -.->|Polled every n seconds| Loader
    Loader -->|Loads & Executes| Snake
    Loader -->|Loads & Executes| OtherGames
    
    Snake -->|Calls| PeriphAPI
    OtherGames -->|Calls| PeriphAPI
    
    Loader -->|Uses| PeriphAPI
    
    PeriphAPI -->|Draws to| OLED
    PeriphAPI -->|Reads state| BTN
```