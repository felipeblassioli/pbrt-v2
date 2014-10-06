Felipe Blassioli
Mauricio Santana 7991170


O codigo do projeto se encontra no link: https://github.com/felipeblassioli/pbrt-v2

Arquivos
========

  * api.cpp : foi modificado para chamar lsystem.h:CreateLSystemShape
  * lsystem.h/lsystem.cpp : o codigo principal do projeto


Execucao
========

Executar o arquivo scenes/lsystem/render.sh para renderizar todos os arquivos
pbrt presentes no diretorio.

Explicacao
==========

O projeto foi baseado nesse link: http://www.allenpike.com/modeling-plants-with-l-systems/

## LSystem::generate()

Com base no axioma, faz as substituicoes adequadas e guarda em generated_system
a string final do LSystem. Tem suporte a condicionais (LSystem estocasticos)

## makePBRTSceneFile

Interpreta a string final de um lsystem no 3D turtle drawing system.
Basicamente faz chamadas aos metodos de api.h

Dificuldades
============

Depois de termos muita dificuldade em implementar o projeto, em particular
nao conseguimos implementar a Primitiva do jeito que gostariamos, como
um grupo de primitivas simples, decidimos chamar os metodos de api.h
diretamente.

Parametros LSystemShape
=======================

  * axiom: axioma do lsystem.
  * rules: producoes.
  * steps: numero de passos na recursao
  * delta: angulo de rotacao

OBS:
Quando ha mais de uma producao para uma letra, eh assumida uma distribuicao uniforme entre as opcoes.
Ver scenes/lsystem/lsystem_stochastic.pbrt

Exemplo
-------

Shape "lsystem" "string axiom" ["A"] "string rules" ["A->[&FL!A]/////`[&FL!A]///////`[&FL!A]" "F->S ///// F" "L->[```^^{-f+f+f-|-f+f+f}]" "S->F L"] "integer steps" [7] "float delta" [22.5]