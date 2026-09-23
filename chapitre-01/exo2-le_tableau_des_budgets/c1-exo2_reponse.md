| Étape | Valeur du chapitre | Valeur mesurée trouvée | Source |

| Les capteurs mesurent le mouvement | 1 à 2 ms | < 3 ms (latence motion-to-photon attribuée à la seule IMU) | Bosch Sensortec, fiche produit de l'IMU BMI085 pour casques VR/AR — https://bosch-sensortec.com/news/imu-bmi085-for-virtual-and-augmented-reality-applications.html |

| Le système transmet la mesure | 1 à 3 ms | ≈ 3 ms| Brevet Meta, Low latency hand-tracking in augmented reality systems — https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/12002168 |

| Votre application décide et dessine | 5 à 11 ms | Introuvable comme valeur mesurée fixe | Documentation développeur Oculus, *Asynchronous TimeWarp (ATW)* — https://developer.oculus.com/documentation/native/android/mobile-timewarp-overview |

| Le compositeur assemble | 1 à 2 ms | Introuvable comme valeur isolée et sourcée. | — (recherche infructueuse) |

| L'écran affiche la ligne | 2 à 5 ms | ≈ 2 ms | Brevet Meta, Low latency hand-tracking in augmented reality systems — https://image-ppubs.uspto.gov/dirsearch-public/print/downloadPdf/12002168 |
