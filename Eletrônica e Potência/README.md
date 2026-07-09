# Semana 2 - Eletrônica e Potência

## Planejamento
- Vamos usar o [Wokwi](https://wokwi.com/projects/new/esp32-c3) para mostrarmos o diagrama de pinagem
- Esse README conterá a documentação da entrega (vamos colocar imagens do diagrama de pinagem e do circuito)

🔋 A alimentação do sistema será realizada por duas baterias de 4,5 V conectadas em série, fornecendo uma diferença de potencial total de 9 V. Como essa tensão excede o recomendado para o ESP, será utilizado um conversor step-down (redutor de tensão). Esse dispositivo permitirá reduzir e regular a tensão de alimentação para 5 V, valor adequado para o funcionamento seguro do circuito.

⚡O ESP será responsável pelo envio dos sinais de controle à ponte H por meio de quatro conexões elétricas. A ponte H, por sua vez, acionará os motores do carrinho, permitindo o controle de seu sentido de rotação e de sua velocidade.

Para garantir melhor condução elétrica, será necessário soldar os pinos metálicos ao ESP utilizando solda à base de estanho. Durante esse processo, tomamos cuidado para que a solda de um pino não encostasse na do seu adjacente nem em outros componentes do ESP, pois isso poderia provocar curtos-circuitos e comprometer o funcionamento do sistema.

## Fixação dos elementos no chassi
### Ponte H

Antes da instalação da ponte H, o chassi já possuía uma elevação projetada especificamente para seu posicionamento, contendo quatro furos para a fixação por meio de quatro parafusos e quatro porcas. Entretanto, essa elevação entrava em contato com os pinos soldados na parte inferior da ponte H. Para evitar esse contato, foram utilizadas porcas como espaçadores entre a elevação do chassi e a ponte H, mantendo-a ligeiramente elevada. Dessa forma, garantiu-se que nenhum componente da placa encostasse no chassi, evitando possíveis interferências mecânicas ou elétricas que poderiam comprometer seu funcionamento.

### Pilhas

Antes da fixação, foi soldado um fio conectando uma pilha à outra, o que exigia que ambas permanecessem posicionadas lado a lado. Em seguida, as pilhas foram acomodadas ao lado da placa padrão e fixadas ao chassi utilizando cola quente

### Placa Padrão

Assim como ocorreu com a ponte H, o chassi já possuía uma elevação destinada ao posicionamento da placa padrão. Para fixá-la nessa região, foi utilizada cola quente, garantindo sua estabilidade. Sobre a placa padrão, será instalado o ESP, que será encaixado em pinos de suporte (headers), permitindo sua remoção, se necessário.

### Motores, encoders e rodas

Conforme previsto no planejamento do chassi, os motores foram fixados na parte inferior por meio de quatro suportes em formato de **T**, cada um com dois furos para a passagem dos parafusos de fixação. Juntamente com os motores, e voltados para a parte interna do chassi, foram aparafusados os encoders, responsáveis pelo monitoramento da rotação das rodas.

As rodas foram encaixadas diretamente nos eixos dos motores. No entanto, como apresentavam uma pequena folga, foi aplicada fita crepe ao redor dos pinos brancos de encaixe, aumentando o atrito entre as peças. Essa solução proporcionou uma fixação mais firme, reduzindo o risco de as rodas se desprenderem durante o deslocamento do carrinho.

## Pinagem e conexões

### Da ponte H ao ESP
### Dos motores a ponte H
### Dos encoders ao ESP