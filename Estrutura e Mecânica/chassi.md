# Planejamento
- Usar o onshape compartilhado
- medir o esp e pensar no tamanho da placa PCB
- pesquisar referências
- importar modelos prontos das rodas, motores e etc.

## Medidas dos componentes
- Placa padrão: 11.4 cm x 8.3 cm x 0.05 cm
- ESP32: 1.7 cm x 2.3 cm x 0.92 cm
- Roda de borracha: 6.5 cm x 6.5 cm x 2.5 cm
- Suporte da pilha: 7.5 cm x 1.9 cm x 2.0 cm
- Roda boba (parte de cima): 3.7 cm x 3.2 cm
- Roda boba (parte de baixo): 4.0 cm x 2.8 cm x 2.5 cm (parte de metal) ou 1.3 cm (roda)
- Motor: 7.1 cm x 1.8 cm (parte amarela) x ou 2.0 cm (parte amarela) 
- Ponte H: 4.3 cm x 4.3 cm x 2.5 cm (parte preta) ou 1.2 cm (parte azul)
- Sensor ultrassônico: 4.5cm x 2.0 cm x 1.4 cm 
- Interruptor: 2.1 cm x 1.5cm x 2.2cm

## Chassi

### Medidas
#### Largura padrão: 14.5cm 
Essa medida foi escolhida com base no tamanho do motor, do pneu e da ponte H (2*4.85 + 4.3) e mais uma folga de 0.5cm

#### Comprimento total: 
1.5cm do parachoque, 7.1cm do motor, 1cm de folga, 11.4 da placa padrão, 1cm do interruptor e 5cm de folga geral = 27cm

#### Detalhes 
Determinamos, em cada lateral do chassi, um retângulo de 2,5 cm × 6,5 cm para acomodar as rodas, garantindo que elas não fiquem excessivamente para fora da estrutura. Logo abaixo desses recortes, foram adicionados pequenos cortes triangulares com o objetivo de evitar qualquer contato entre o material do chassi e as rodas durante o movimento.

Ao lado dos recortes maiores, encontram-se quatro pequenos furos: dois mais próximos da borda e dois mais centralizados. A distância entre cada par de furos corresponde à largura do motor responsável por acionar as rodas de borracha. O projeto prevê a utilização de peças em formato de “T”, que serão encaixadas nesses furos para a fixação do motor. As posições dos furos foram definidas de modo a permitir que o motor seja aparafusado em ambos os lados da estrutura.

No centro da parte posterior do chassi, foi projetada uma abertura quadrada no chassi acompanhada de uma elevação perimetral (medida quadrado externo: 4.4, medida do quadrado interno: 3.1), desenvolvida com o objetivo de permitir que a ponte H fosse parafusada em uma posição elevada em relação ao plano principal do chassi de modo a garantir que a parte inferior, com soldas, não encostasse em nada que pudesse prejudicar seu funcionamento.

À frente da ponte H será posicionado o sensor ultrassônico. Como seus pinos de conexão e fios ficarão voltados para a parte traseira do carrinho, não será necessária a criação de uma segunda elevação para acomodá-los.

Por questões estéticas, uma vez que o sensor funciona como os “olhos” do robô, foram adicionados elementos circulares ao redor de sua posição. Além de contribuir para a aparência do projeto, esses elementos auxiliam na fixação e no correto posicionamento do sensor.

Sua localização foi escolhida de forma a minimizar a distância até possíveis obstáculos presentes na trajetória do carrinho, aumentando a rapidez e a precisão da detecção. Adicionalmente, tomou-se o cuidado de não posicionar nenhum componente à frente do sensor, evitando interferências em seu campo de visão e garantindo o pleno funcionamento do sistema.

Atrás da ponte H, observa-se a presença de um recorte retangular e dois furos circulares. Esses elementos foram projetados para acomodar o interruptor e permitir a passagem dos fios de alimentação conectados a ele.

As dimensões adotadas no projeto foram definidas com base nas medidas obtidas experimentalmente no laboratório, garantindo a compatibilidade entre o modelo desenvolvido e os componentes físicos utilizados na montagem do carrinho.