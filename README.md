<div align="center">

# Git Comando

**Robô autônomo com Arduino, feito pela equipe Git Comando para o desafio de robótica do componente curricular de PAC (Projeto de Aprendizagem Colaborativa).**

</div>

---

## Sobre o projeto

No sumô de robôs, o objetivo é encontrar o adversário na arena e empurrá-lo para fora, sem deixar que o próprio robô ultrapasse a linha branca da borda.

Pela regra do desafio, o robô só pode começar a agir **5 segundos depois de ligado**. O liga/desliga é feito por controle remoto infravermelho (IR).

---

## Como o robô decide

A cada ciclo do `loop()`, o robô segue uma ordem de prioridade:

1. **Linha branca (borda):** freia, dá ré e gira para voltar à arena.
2. **Oponente perto:** avança em velocidade máxima para atacar.
3. **Nada detectado:** gira procurando o adversário.

Na largada, o robô também avança por um curto tempo (`tempoArrancada`), parando antes se encontrar a borda.

---

## Hardware

| Componente | Pino |
|---|---|
| Motor A (esquerdo): ENA / IN1 / IN2 | 5 / 6 / 7 |
| Motor B (direito): ENB / IN3 / IN4 | 3 / 2 / 4 |
| Sensor de proximidade (Sharp) | A0 |
| Sensor de linha esquerdo (SE) | A1 |
| Sensor de linha direito (SD) | A2 |
| LED RGB (R / G / B) | 9 / 10 / 11 |
| Receptor IR | 8 |

---

## Estrutura do repositório

```
git-comando-sumo/
├── src/
│   ├── main.ino                 # Template com os TODOs da estratégia
│   └── git-comando-alpha.ino    # Versão com a estratégia implementada
└── tests/
    ├── teste2.1.ino
    ├── teste3/
    │   └── teste3.ino
    └── teste-git-comando-frontend/
        └── PERSEGUIDOR_GIT_COMANDO.ino
```

---

## Como rodar

1. Instale a biblioteca **IRremote** na Arduino IDE.
2. Abra o arquivo `.ino` que deseja usar (`src/git-comando-alpha.ino` para a estratégia pronta).
3. Selecione a placa e a porta e faça o upload.

> O `git-comando-alpha.ino` está atualmente sem a biblioteca IRremote, para testes.

---

## Parâmetros ajustáveis (`git-comando-alpha.ino`)

| Parâmetro | Valor | Função |
|---|---|---|
| `limiteBranco` | 200 | Abaixo disso = branco (borda) |
| `limiteOponente` | 400 | Acima disso = oponente perto |
| `velocidade` | 140 | Velocidade andando |
| `velocidadeGiro` | 180 | Velocidade no giro de fuga |
| `velocidadeAtaque` | 255 | Velocidade no ataque |
| `velocidadeBusca` | 160 | Velocidade do giro de busca |
| `tempoFreio` | 150 ms | Duração do freio na borda |
| `tempoRe` | 450 ms | Ré quando os dois sensores veem branco |
| `tempoReLado` | 400 ms | Ré quando só um sensor vê branco |
| `tempoGiro180` | 250 ms | Giro quando os dois sensores veem branco |
| `tempoGiroLado` | 220 ms | Giro quando só um sensor vê branco |
| `tempoArrancada` | 1000 ms | Tempo andando para frente na largada |

---

## Testes

Os arquivos em `tests/` são usados para validar o hardware antes de rodar a estratégia completa.

> **Conflito de timer:** o PWM do motor direito (pino 3) e o receptor IR usam o Timer2 do Arduino. Por isso o `teste3.ino` define `#define IR_USE_AVR_TIMER1` antes do `#include <IRremote.hpp>`. Não apague essa linha.

---

## Equipe

<table align="center">
  <tr>
    <td align="center">
      <a href="https://github.com/BeatrizCercal">
        <img src="https://github.com/BeatrizCercal.png" width="100" style="border-radius:50%" alt="BeatrizCercal"><br>
        <sub><b>BeatrizCercal</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/hemkdev">
        <img src="https://github.com/hemkdev.png" width="100" style="border-radius:50%" alt="hemkdev"><br>
        <sub><b>hemkdev</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/miguelrcha">
        <img src="https://github.com/miguelrcha.png" width="100" style="border-radius:50%" alt="miguelrcha"><br>
        <sub><b>miguelrcha</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/enzoronchii">
        <img src="https://github.com/enzoronchii.png" width="100" style="border-radius:50%" alt="enzoronchii"><br>
        <sub><b>enzoronchii</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/DeveloperNatan">
        <img src="https://github.com/DeveloperNatan.png" width="100" style="border-radius:50%" alt="DeveloperNatan"><br>
        <sub><b>DeveloperNatan</b></sub>
      </a>
    </td>
  </tr>
</table>
