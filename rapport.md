# Rapport - Laboratoire 4

### Auteurs : Rafael Dousse & Benoît Delay

## Introduction

Ce laboratoire vise à développer un système d'acquisition multimédia capable de traiter et d'enregistrer des données audio et vidéo dans un environnement temps-réel. Nous avons du intégrer des données audio collectées via un microphone connecté à l'entrée "MIC IN" de la FPGA et retourner à la sortie "MIC OUT", simulant ainsi une production continue d'échantillons sonores. En parallèle, des données vidéo sont extraites d'un fichier et affichées sur un écran. L'objectif est de concevoir un système qui lit, enregistre et affiche ces flux de données de manière efficace, utilisant Xenomai pour répondre aux exigences strictes de temps réel.

## Etape 1 : Audio

Pour determiner la periode de la tache Audio j'ai utilisé le calcul suivant
$$ periode = {1 \over (frequence*echantillonage * nbCanaux \_ sizeof(uint16_t) / taille_buffer)}s $$

Le calcul s'explique de la facon suivante:

- On prends la frequence d'echantillonage
- On la multiplie par le nombre de canal
- On la mutliplie par 2 car l'information est sur 2 octes

Avec ceci nous obtenons la fréquence a laquelle il faudrait lire byte par byte. Néanmoins, vu que nous avons 2 buffer de 128 byte nous pouvons lire tous les 256 bytes le buffer donc nous divisions cette frequence par 256.

Puis vu que $$ temps = {1 \over frequence}$$

On applique cette formule et nous obtenons 1.3ms.

Etant donné que la fonction `rt_task_set_periodic` demande une période en nanoseconde nous effectuons la conversion ce qui donne 1333333 ns

## Etape 2 : Video

Cette étape a pour but de mettre en place la tâche vidéo qui va lire un fichier `.raw` et afficher les images sur l'écran. La vidéo a une durée d'à peu près 20 secondes. Pour avoir une lecture fluide est pas trop rapide ou lent, il faut faire quelque calculs. La données indique que le format de la vidéo est est de 320 pixels par 240 pixels, ou chaque pyxel a une configuration RGB0 sur 4 byte. On nous indique aussi un framerate de 15 images par secondes. Avec toutes ses données, on peut calculer le temps que doit prendre une tâche pour afficher une image et la taille nécessaire à allouer. Ainsi, les calculs que nous avons fait sont les suivant:

- On sait qu'on doit avoir 15 images par secondes, donc une image doit être affichée toutes les 1/15 secondes ce qui donne 0.066 s ou 66666666 ns.

- La mémoire alloué qui est nécessaire pour affiché une frame a été calculé de cette manière: 320 _ 240 _ 4 nous donne un total de 307200 bytes qui est aussi la taille d'une image.

Pour savoir combien de frame contient le fichier et la durée de la vidéo, on peut faire le calcul suivant:
La taille du fichier vidéo est de 87.9 MB ce qui donne plus ou moins 92169830.4 bytes. Ainsi, la taille total du fichier divisé par la taille pour une frame nous donne le nombres de frame: 92169830.4 / 307200 = 300 frames. Finalement, on peut calculer la durée de la vidéo en secondes: 300 / 15 = 20 secondes.

## Etape 3 : Caractérisation des tâches


### Mesure

- Audio:

  Total of 50 values 
    Minimum  = 1306350.000000 (position = 0) 
    Maximum  = 1353670.000000 (position = 12) 
    Sum      = 66611700.000000 
    Mean     = 1332234.000000 
    Variance = 75105432.000000 
    Std Dev  = 8666.339019 
    CoV      = 0.006505 

- Audio avec enregistrement

  Total of 50 values 
    Minimum  = 1166800.000000 (position = 5) 
    Maximum  = 2478740.000000 (position = 23) 
    Sum      = 67056970.000000 
    Mean     = 1368509.591837 
    Variance = 64876166248.811768 
    Std Dev  = 254708.001933 
    CoV      = 0.186121 

- Video

  Total of 50 values 
    Minimum  = 66631740.000000 (position = 0) 
    Maximum  = 66673860.000000 (position = 22) 
    Sum      = 3266541440.000000 
    Mean     = 66664111.020408 
    Variance = 93504727.500000 
    Std Dev  = 9669.784253 
    CoV      = 0.000145 

- Video avec enregistrement

Total of 50 values 
    Minimum  = 66635050.000000 (position = 14) 
    Maximum  = 66680460.000000 (position = 48) 
    Sum      = 3266592930.000000 
    Mean     = 66665161.836735 
    Variance = 75566101.000000 
    Std Dev  = 8692.876451 
    CoV      = 0.000130 

## Problème rencontré

Son qui se coupe quand enregistre. On a mit sur une cpu chacun.

On a aussi on problème des délais dépassé. On pourrait changé la priorité de la tâche audio pour qu'elle soit plus haute que la tâche vidéo. (genre set affinity ou le 99 de rt*task_create(&audio, "Audio Timer", 0, 99, T*))
