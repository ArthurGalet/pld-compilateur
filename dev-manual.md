# PLD Compilateur : Developper's manual

## Instructions en représentation intermédiaire (IR)

| Nom de l'instruction | Paramètre(s)                                                   | Description                                                                                          |
|----------------------|----------------------------------------------------------------|------------------------------------------------------------------------------------------------------|
| loadconst            | 0 : une variable<br/> 1 : la valeur                            | Met une valeur entière dans une variable                                                             |
| copyvar              | 0 : une variable <br/> 1 : une variable                        | Permet de copier le contenu de la variable `1` dans la variable `0`.                                 |
| incr                 | 0 : une variable <br/>                                         | Ajoute 1 à la variable                                                                               |
| decr                 | 0 : une variable <br/>                                         | Enlève 1 à la variable                                                                               |
| add                  | 0 : une variable <br/> 1 : une variable <br/> 2 : une variable | Met la somme des variables `1` et `2` dans la `0`.                                                   |
| sub                  | 0 : une variable <br/> 1 : une variable <br/> 2 : une variable | Met le résultat de variable `1` - variable `2` dans la variable `0`.                                 |
| mul                  | 0 : une variable <br/> 1 : une variable <br/> 2 : une variable | Met le produit des variables `1` et `2` dans la `0`.                                                 |
| divide               | 0 : une variable <br/> 1 : une variable <br/> 2 : une variable | Met le résultat de variable `1` divisée par la variable `2` dans la variable `0`.                    |
| modulo               | 0 : une variable <br/> 1 : une variable <br/> 2 : une variable | Met le résultat de variable `1` modulo la variable `2` dans la variable `0`.                         |
| neg                  | 0 : une variable <br/>                                         | Inverse le signe de la variable                                                                      |
| call                 | 0 : un nom de fonction <br/> 1 .. n : des variables            | Appelle la fonction nommée par le paramètre `0` avec les paramètres `1 .. n`                         |
| cmp_eq               | 0 : une variable <br/> 1 : une variable <br/> 2 : une variable | Vérifie si la variable `1` est égale à la `2` et met le résultat dans la variable `0`.               |
| cmp_neq              | 0 : une variable <br/> 1 : une variable <br/> 2 : une variable | Vérifie si la variable `1` est différente de la `2` et met le résultat dans la variable `0`.         |
| cmp_lt               | 0 : une variable <br/> 1 : une variable <br/> 2 : une variable | Vérifie si la variable `1` est inférieure à la `2` et met le résultat dans la variable `0`.          |
| cmp_le               | 0 : une variable <br/> 1 : une variable <br/> 2 : une variable | Vérifie si la variable `1` est inférieure ou égale à la `2` et met le résultat dans la variable `0`. |
| cmp_gt               | 0 : une variable <br/> 1 : une variable <br/> 2 : une variable | Vérifie si la variable `1` est supérieure à la `2` et met le résultat dans la variable `0`.          |
| cmp_ge               | 0 : une variable <br/> 1 : une variable <br/> 2 : une variable | Vérifie si la variable `1` est supérieure ou égale à la `2` et met le résultat dans la variable `0`. |
| lnot                 | 0 : une variable <br/>                                         | Effectue un non logique sur la variable                                                              |
| bwor                 | 0 : une variable <br/> 1 : une variable <br/> 2 : une variable | Met le résultat du OU binaire entre les variables `1` et `2` dans la `0`.                            |
| bwand                | 0 : une variable <br/> 1 : une variable <br/> 2 : une variable | Met le résultat du ET binaire entre les variables `1` et `2` dans la `0`.                            |
| bwxor                | 0 : une variable <br/> 1 : une variable <br/> 2 : une variable | Met le résultat du OU EXCLUSIF binaire entre les variables `1` et `2` dans la `0`.                   |
| bwnot                | 0 : une variable                                               | Applique un NON binaire à la variable `0`.                                                           |
| bwsl                 | 0 : une variable <br/> 1 : une variable <br/> 2 : une variable | Met le résultat du décalage à gauche binaire entre les variables `1` et `2` dans la `0`.             |
| bwsr                 | 0 : une variable <br/> 1 : une variable <br/> 2 : une variable | Met le résultat du décalage à droite binaire entre les variables `1` et `2` dans la `0`.             |
| jump                 | 0 : un nom de basic bloc                                       | Se déplace vers le basic bloc susnommé                                                               |
| ret                  | 0 : une variable <br/>                                         | Retourne la variable et met fin à la fonction en cours                                               |


## Visiteurs

### `ValidatorVisitor`

Cette classe se charge de faire toutes les vérifications qu'on ne peut pas faire dans la grammaire mais qui peuvent empêcher la compilation.

###  `CToIRVisitor` 

Cette classe se charge de faire la traduction du language C en représentation intermédiaire (IR).
Elle crée un `CFG` par fonction et génère tous les `BasicBlock` et les remplis d'instructions. 
Elle se charge d'attribuer l'offset sur la pile à chaque variable.


### `IROptimizer`

Cette classe se charge de simplifier des suites d'instructions IR. 
Les instructions étant générées une à une sans tenir compte des autres, beaucoup d'opérations sont effecturées de manière très peu efficace.

La classe vient lire une instruction IR ainsi que celles qui la suivent, et si on est dans un cas qui peut être simplifié, ont remplace ces instructions par une version plus optimale.