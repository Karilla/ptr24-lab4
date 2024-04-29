# Rapport - Laboratoire 4
### Auteurs : Rafael Dousse & Benoît Delay

## Introduction

Ce laboratoire vise à développer un système d'acquisition multimédia capable de traiter et d'enregistrer des données audio et vidéo dans un environnement temps-réel. Nous avons du intégrer des données audio collectées via un microphone connecté à l'entrée "MIC IN" de la FPGA et retourner à la sortie "MIC OUT", simulant ainsi une production continue d'échantillons sonores. En parallèle, des données vidéo sont extraites d'un fichier et affichées sur un écran. L'objectif est de concevoir un système qui lit, enregistre et affiche ces flux de données de manière efficace, utilisant Xenomai pour répondre aux exigences strictes de temps réel.

## Etape 1 : Audio

1 / ((frequence echantillonae * nbCanaux * nbByte)/ tailleBuffer )


## Etape 2 : Video

Cette étape a pour but de mettre en place la tâche vidéo qui va lire un fichier `.raw` et afficher les images sur l'écran. La vidéo a une durée d'à peu près 20 secondes. Pour avoir une lecture fluide est pas trop rapide ou lent, il faut faire quelque calculs. La données indique que le format de la vidéo est est de 320 pixels par 240 pixels, ou chaque pyxel a une configuration RGB0 sur 4 byte. On nous indique aussi un framerate de 15 images par secondes. Avec toutes ses données, on peut calculer le temps que doit prendre une tâche pour afficher une image et la taille nécessaire à allouer. Ainsi, les calculs que nous avons fait sont les suivant:

- On sait qu'on doit avoir 15 images par secondes, donc une image doit être affichée toutes les 1/15 secondes ce qui donne 0.066 s ou 66666666 ns.

- La mémoire alloué qui est nécessaire pour affiché une frame a été calculé de cette manière: 320 * 240 * 4 nous donne un total de  307200 bytes qui est aussi la taille d'une image.

Pour savoir combien de frame contient le fichier et la durée de la vidéo, on peut faire le calcul suivant:
La taille du fichier vidéo est de 87.9 MB ce qui donne plus ou moins 92169830.4 bytes. Ainsi, la taille total du fichier divisé par la taille pour une frame nous donne le nombres de frame: 92169830.4  / 307200 = 300 frames. Finalement, on peut calculer la durée de la vidéo en secondes: 300 / 15 = 20 secondes.

## Etape 3 : Caractérisation des tâches



## Problème rencontré

Son qui se coupe quand enregistre. On a mit sur une cpu chacun. 

On a aussi on problème des délais dépassé. On pourrait changé la priorité de la tâche audio pour qu'elle soit plus haute que la tâche vidéo. (genre set affinity ou le 99 de  rt_task_create(&audio, "Audio Timer", 0, 99, T_))
