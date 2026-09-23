
| Angle     | Tangente | Degrés |

| angleLeft | 1,165 | 49,4° |
| angleRight| 0,993 | 44,8° |
| angleUp   | 1,056 | **46,6° |
| angleDown | 1,059 | 46,6° |

FOV horizontal total (gauche + droite) : 94,2° — FOV vertical total (haut + bas) : 93,2°.

Source

Fil de discussion du forum de *Live for Speed* (simulateur de course), où l'auteur du module VR du jeu publie la sortie brute de `LFSVR_QueryHMD` obtenue sur un HP Reverb G2, puis un autre intervenant convertit les tangentes en degrés par arctangente.
https://lfs.net/forum/post/1962715

Recoupement : Tom's Hardware, dans son test du Reverb G2, mesure lui aussi un angle horizontal effectif "plus proche de 90°" que les 114° annoncés par HP (qui est une valeur marketing, pas la mesure par œil) — cohérent avec le total de 94,2° retrouvé ici. https://www.tomshardware.com/uk/reviews/hp-reverb-g2

Pourquoi ce n'est pas symétrique

Le fil de discussion explique que l'angle le plus grand du côté "left" reflète le fait que les lentilles sont décalées vers l'intérieur (vers le nez) par rapport à l'axe de l'œil, pas centrées dessus.

En une phrase : et avec un champ symétrique de même surface ?

Un champ symétrique de même surface recentrerait le rendu sur l'axe de la tête plutôt que sur l'axe réel de chaque lentille (décalée vers le nez) : la surface totale affichée serait la même, mais l'image ne serait plus alignée avec l'endroit où l'œil regarde réellement à travers l'optique du casque, et le bord serait visiblement faux, sans qu'aucune erreur ne soit signalée nulle part — exactement le troisième piège nommé dans le chapitre.
