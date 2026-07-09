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

### Baterias
### Placa Padrão


## Pinagem e conexões

