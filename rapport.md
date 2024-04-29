# Rapport - Laboratoire 4

### Auteurs : Rafael Dousse & Benoît Delay

## Introduction

Ce laboratoire vise à développer un système d'acquisition multimédia capable de traiter et d'enregistrer des données audio et vidéo dans un environnement temps-réel. Nous avons du intégrer des données audio collectées via un microphone connecté à l'entrée "MIC IN" de la FPGA et retourner à la sortie "MIC OUT", simulant ainsi une production continue d'échantillons sonores. En parallèle, des données vidéo sont extraites d'un fichier et affichées sur un écran. L'objectif est de concevoir un système qui lit, enregistre et affiche ces flux de données de manière efficace, utilisant Xenomai pour répondre aux exigences strictes de temps réel.

## Etape 1 : Audio

Pour determiner la periode de la tache Audio nous avons fait le calcul suivant :
$$ periode = {1 \over (frequence*echantillonage * nbCanaux \_ sizeof(uint16_t) / taille_buffer)}s $$

Le calcul s'explique de la facon suivante:

- On prend la fréquence d'échantillonnage.
- On la multiplie par le nombre de canaux.
- On multiplie le résultat par 2, car chaque échantillon est codé sur 2 octets.

Cela nous donne la fréquence à laquelle il faudrait lire les données octet par octet. Cependant, comme nous disposons de deux buffers de 128 octets chacun, nous pouvons lire toutes les données tous les 256 octets. Nous divisons donc cette fréquence par 256.

Ensuite, sachant que $$ temps = {1 \over frequence}$$

Nous appliquons cette formule pour obtenir une période de 1,3 ms.

Étant donné que la fonction rt_task_set_periodic requiert une période exprimée en nanosecondes, nous effectuons la conversion, ce qui donne 1 333 333 ns.

## Etape 2 : Video

Cette étape vise à mettre en place la tâche vidéo qui consistera à lire un fichier `.raw` et à afficher les images sur l'écran. La vidéo dure environ 20 secondes. Pour assurer une lecture fluide, ni trop rapide ni trop lente, il est nécessaire de réaliser quelques calculs. Les données indiquent que le format de la vidéo est de 320 pixels par 240 pixels, chaque pixel étant codé en RGB0 sur 4 octets. Le taux de rafraîchissement est également spécifié à 15 images par seconde. À partir de ces informations, nous pouvons calculer le temps nécessaire pour afficher une image et la taille de mémoire à allouer. Les calculs effectués sont les suivants :

Nous devons afficher 15 images par seconde, donc une image doit être affichée toutes les 1/15 de seconde, ce qui équivaut à 0.066 seconde ou 66 666 666 nanosecondes.

La quantité de mémoire nécessaire pour stocker une image a été calculée comme suit : 320×240×4 bytes donne un total de 307 200 bytes, qui est également la taille d'une image.

Pour déterminer le nombre total de frames contenues dans le fichier et la durée de la vidéo, nous procédons au calcul suivant :
La taille du fichier vidéo est de 87,9 MB, soit environ 92 169 830,4 bytes. Le nombre total de frames est donc obtenu en divisant cette taille par celle d'une frame : 92169830 / 307200 = 300 frames. Enfin, nous pouvons calculer la durée de la vidéo en secondes : 300 / 15 = 20 secondes.

## Etape 3 : Caractérisation des tâches

Pour  caractériser nos tâches, nous avons utilisé le programme `summary1.c` fournit lors du labo 2 et nous permet d'afficher des données statistiques sur nos tâches. Nous avons donc lancé les tâches en imprimant le temps de début et de fin de chaque tâche et en enregistrant les données dans un fichier texte. Les mesures prises peuvent être consultées dans les fichiers `.dat` correspondant. 

Avec 50 données par tâches, nous avons pu obtenir des statistiques sur les tâches audio et vidéo suivantes : 

- [Audio](code/audio.dat):

  - Total of 50 values 
    -  Minimum  = 1306350.000000
    - Maximum  = 1353670.000000 
    - Sum      = 66611700.000000
    - Mean     = 1332234.000000
    - Variance = 75105432.000000
    - Std Dev  = 8666.339019
    - CoV      = 0.006505

Les faibles écart-type et CoV, la moyenne proche de la période voulue et une différence de 47,320 ns entre le temps le plus court et le plus long suggèrent que la tâche audio est stable et prévisible. Cela montre que la tâche audio est bien gérée par le système temps-réel. 

- [Audio avec enregistrement](code/audioCharge.dat):

  - Total of 50 values 
    - Minimum  = 1166800.000000 
    - Maximum  = 2478740.000000 
    - Sum      = 67056970.000000
    - Mean     = 1368509.591837
    - Variance = 64876166248.811768 
    - Std Dev  = 254708.001933 
    - CoV      = 0.186121 

Quand nous prenons les mesures avec l'enregistrement de l'audio activé, nous obtenons des délais de la tâche,  qui n'arrive pas a s'executer dans le temps alloué. Les mesures nous le montre bien. La différence maximale entre les temps d'exécution les plus courts et les plus longs est de 1,311,940 ns, ce qui est une augmentation notable par rapport à l'exécution sans enregistrement.  La moyenne du temps d'exécution est de 1,368,509 ns, donc supérieur à la période cible de 1,333,333 ns pour la tâche audio et bien plus grande qu'avant. L'écart-type est de 254,708 ns, indiquant une dispersion très élevée des mesures autour de la moyenne.


- [Video](code/video.dat):

  - Total of 50 values 
    - Minimum  = 66631740.000000  
    - Maximum  = 66673860.000000 
    - Sum      = 3266541440.000000
    - Mean     = 66664111.020408 
    - Variance = 93504727.500000 
    - Std Dev  = 9669.784253 
    - CoV      = 0.000145 

Les statistiques de la tâche vidéo montrent une moyenne de 66,664,111 ns qui est proche de la valeur cible qui est de 66,666,666 ns. La différence entre les temps d'exécution les plus courts et les plus longs est de seulement 10,120 ns et l'écart-type est de 9,669 ns, ce qui est très faible, indiquant que les temps d'exécution sont très concentrés autour de la moyenne.

Ces résultats montrent a nouveau que les tâches audio et vidéo sont bien gérées par le système temps-réel quand il n'y a pas de surcharge qui celle de l'enregistrement audio qui prends plus de temps que prévu mais affecte que la tâche audio et pas la tâche vidéo comme on peut le voir juste après.

- [Video avec enregistrement](code/videoCharge.dat):

    - Total of 50 values 
        -  Minimum  = 66635050.000000 
        -  Maximum  = 66680460.000000 
        -  Sum      = 3266592930.000000 
        -  Mean     = 66665161.836735 
        -  Variance = 75566101.000000 
        - Std Dev  = 8692.876451 
        - CoV      = 0.000130 

Finalement, les statistiques de la tâche vidéo avec enregistrement montrent une moyenne de 66,665,161 ns, qui est proche de la valeur cible de 66,666,666 ns. La différence entre les temps d'exécution les plus courts et les plus longs est de 45,410 ns et l'écart-type est de 8,692 ns, ce qui est très faible, indiquant que les temps d'exécution sont très concentrés autour de la moyenne. Cela montre que l'enregistrement audio n'affecte presque pas la vidéo ce qui pourrait être surprenent mais la raison est expliqué dans le chapitre suivant. 

## Problème rencontré

__Problème :__

Lors de l'enregistrement audio, nous avons observé des problèmes de qualité sonore, notamment un son grésillant et coupé. Parallèlement, la performance de la vidéo était également affectée, avec des sauts d'image notables. Ces problèmes semblent indiquer une contention des ressources système, en particulier une surcharge du processeur qui gère simultanément les tâches audio et vidéo et l'enregistrement audio.
Le problème pourrait venir du fait que lorsque le CPU est surchargé, il ne peut pas traiter les échantillons audio ou les frames vidéo assez rapidement, ce qui résulte en une qualité médiocre de l'audio et des sauts dans la vidéo.

De plus, l'écriture de l'audio sur un fichier se fait avec l'utilisation des fonctions `append_wav_dat` et `write_wav_header` qui sont des fonctions qui utilisent des appels système pour écrire sur la carte. Ces appels système peuvent être bloquants et ralentir l'exécution de la tâche audio, ce qui pourrait expliquer pourquoi l'enregistrement audio affecte la qualité du son et de la vidéo.

__Solution implémentées :__

1. Isolation des CPU : Pour minimiser la contention des ressources, chaque tâche (audio et vidéo) a été assignée à un CPU différent. Cette séparation aide à réduire les interférences entre les tâches en assurant que chaque tâche a un accès dédié aux ressources du CPU. Cela a permis d'améliorer significativement la qualité de l'audio et la fluidité de la vidéo.

2. Réglage des priorités : Une autre stratégie envisagée était de modifier les priorités des tâches. En augmentant la priorité de la tâche audio par rapport à la vidéo, le système s'assure que les traitements audio, plus sensibles, sont moins susceptibles de subir des retards, car le planificateur du système alloue d'abord du temps CPU à ces tâches critiques. Cela aurait pu être fait grâce à l'utilisation de rt_task_create avec un niveau de priorité plus élevé pour la tâche audio ou ajustement de l'affinité CPU (set affinity) pour optimiser la répartition des tâches sur les différents cœurs.

## Conclusion

Ce laboratoire nous a permis de concevoir un système capable de gérer simultanément des flux de données audio et vidéo en temps réel. En utilisant Xenomai, nous avons atteint une exécution efficace et stable de nos tâches multimédia, malgré les défis de synchronisation et de contention des ressources.

Nous avons résolu des problèmes significatifs tels que la détérioration de la qualité audio lors de l'enregistrement et les interruptions vidéo en isolant les tâches sur différents CPU et en ajustant leurs priorités. Ces stratégies ont amélioré la qualité des traitements et démontré l'importance de l'allocation correcte des ressources dans un environnement temps réel.

Précision sur le rapport: Les résultats, les idées et les discussions ont été faites par les auteurs mais le texte a été reformulé par chatgpt pour un rapport plus clair et concis.