# Como testar o robô

Rodar na ordem. Serial Monitor sempre em **9600**.

## T1 - motores e sensores (`t1_teste`)

**Primeiro os motores.** Subir o código com o robô levantado (rodas no ar) e a bateria ligada.
Depois de 3 s ele gira cada roda pra frente e pra trás, e o Serial Monitor mostra o que deveria
estar acontecendo.

- Uma roda girou ao contrário do que diz a tela: inverter os dois fios desse motor na ponte H.
- "Esquerdo" moveu a roda direita: os motores estão trocados na ponte H.
- A roda quase não gira: aumentar o `velocidade` no T2.
- Pra ver de novo: botão RESET do Arduino.

**Depois os sensores.** Quando aparecer "Pode colocar o robô no chão", a tela passa a mostrar os
valores dos sensores sem parar.

1. Sensores de linha no **preto**: anotar os valores do Esquerdo e do Direito.
2. No **branco**: anotar de novo.
3. Distância sem nada na frente e com um objeto a uns 20 cm: anotar.

O que tirar daqui:
- O `line` do T2 é o **meio** entre o valor do preto e o do branco.
- Se o branco der um número **maior** que o preto, trocar o `<` por `>` na função `branco()` do T2.
- Se o preto e o branco derem valores quase iguais, o sensor está alto demais, sujo ou com mau contato.

## T2 - arena (`t2_arena`)

É o código do professor preenchido. Liga com qualquer controle, espera os 5 s e anda pra
frente. Quando um sensor vê branco, dá ré e gira pro lado contrário.

Antes de subir, ajustar no topo do arquivo o `line` (do T1) e, se precisar, o `<` da função
`branco()`.

Se der errado:
- Fica dando ré e girando no meio do preto: o `line` ou o `<`/`>` estão errados. Voltar ao T1.
- Passa do branco e sai da arena: diminuir o `velocidade`.
- Gira muito pouco e volta pra borda: aumentar o `delay(350)` do giro.

Tem uma linha a mais no topo do T2, `#define IR_USE_AVR_TIMER1`. Sem ela o motor direito trava o
controle IR, porque os dois usam o mesmo timer do Arduino. Não apagar.

## O que mandar de volta

Os valores anotados no T1, o que aconteceu com as rodas e um vídeo do T2.
