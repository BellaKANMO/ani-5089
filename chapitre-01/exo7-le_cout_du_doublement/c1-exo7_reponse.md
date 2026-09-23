
|                               |Moyenne   | Plus longue (pire cas) |

| Rendu x1                      | 0.87 ms  | 1.91 ms                |
| Rendu x2 estimé               | 1.74 ms  | 4.01 ms (1.91 * 2.1)   |
| Images > 11ms (rendu seul x2) | 0 / 1000 |                        |

Conclusion : Même fait deux fois le rendu ne prend que 4 ms max sur les 11 ms donc il tient largement et ce n'est pas le rendu qu'il faut réduire mais les pics du système et le temps de la logique.