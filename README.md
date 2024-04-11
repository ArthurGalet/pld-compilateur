# pld-compilateur

Compilateur C fait maison

## Utilisation

Un Makefile est disponible dans le dossier `compiler`
Il dispose des targets suivantes :
- `ifcc` ou `all` (règle par défaut) : compile le compilateur
- `clean` : supprime les fichiers
- `test` : exécute tout les tests du dossier `tests/testfiles`. /!\ La target ifcc est une dépendance de cette target.

Pour compiler un programme, il suffit d'utilisater la commande suivante : `./ifcc <fichier.c>`.
Le compilateur ne dispose pas d'options en ligne de commande.

Le fichier `dev-manual.md` décrit plus en détail le projet.

## Liste des fonctionnalités

| Fonctionnalité                                                                                                       | Priorité | Développement  |
|----------------------------------------------------------------------------------------------------------------------|----------|----------------|
| Type de données de base int (un type 32 bits)                                                                        | O        | Terminé        |
| Variables                                                                                                            | O        | Terminé        |
| Constantes entières et caractère (avec simple quote)                                                                 | O        | Terminé        |
| Opérations arithmétiques de base : +,-, *                                                                            | O        | Terminé        |
| Division et modulo                                                                                                   | O        | Terminé        |
| Opérations logiques bit-à-bit : \|, &,ˆ                                                                              | O        | Terminé        |
| Opérations de comparaison : ==, !=, <, >                                                                             | O        | Terminé        |
| Opérations unaires : ! et -                                                                                          | O        | Terminé        |
| Déclaration de variables n’importe où                                                                                | O        | Terminé        |
| Affectation (qui, en C, retourne aussi une valeur)                                                                   | O        | Terminé        |
| Utilisation des fonctions standard putchar et getchar pour les entrées-sorties                                       | O        | Terminé        |
| Définition de fonctions avec paramètres, et type de retour int ou void                                               | O        | Terminé        |
| Vérification de la cohérence des appels de fonctions et leurs paramètres                                             | O        | Terminé        |
| Structure de blocs grâce à { et }                                                                                    | O        | Terminé        |
| Support des portées de variables et du shadowing                                                                     | O        | Terminé        |
| Les structures de contrôle if, else, while                                                                           | O        | Terminé        |
| Support du return expression n’importe où                                                                            | O        | Terminé        |
| Vérification qu’une variable utilisée a été déclarée                                                                 | O        | Terminé        |
| Vérification qu’une variable n’est pas déclarée plusieurs fois                                                       | O        | Terminé        |
| Vérification qu’une variable déclarée est utilisée                                                                   | O        | Terminé        |
| Reciblage vers plusieurs architectures : x86, MSP430, ARM                                                            | F        | Non implémenté |
| Support des double avec toutes les conversions implicites                                                            | F        | Non implémenté |
| Propagation de constantes simple                                                                                     | F        | Terminé        |
| Propagation de variables constantes (avec analyse du data-flow)                                                      | F        | Terminé        |
| Tableaux (à une dimension)                                                                                           | F        | Non implémenté |
| Pointeurs                                                                                                            | F        | Non implémenté |
| break et continue                                                                                                    | F        | Terminé        |
| Support en largeur du type de données char (entier 8 bits)                                                           | F        | Non implémenté |
| Les chaînes de caractères représentées par des tableaux de char                                                      | F        | Non implémenté |
| Possibilité d’initialiser une variable lors de sa déclaration                                                        | F        | Terminé        |
| switch...case                                                                                                        | F        | Non implémenté |
| Les opérateurs logiques paresseux \|\|, &&                                                                           | F        | Terminé        |
| Opérateurs d’affectation +=, -= etc., d’incrémentation ++ et décrémentation --                                       | F        | Terminé        |
| Les variables globales                                                                                               | NP       | Non implémenté |
| Les autres types de inttypes.h, les double                                                                           | NP       | Non implémenté |
| Le support dans les moindres détails de tous les autres opérateurs arithmétiques et logiques : <=, >=, << et >> etc. | NP       | Terminé        |
| Les autres structures de contrôle : for, do...while                                                                  | NP       | Terminé        |
| La possibilité de séparer dans des fichiers distincts les déclarations et les définitions                            | D        | Non implémenté |
| Le support du préprocesseur (#define, #include, #if, etc.)                                                           | D        | Non implémenté |
| Les structures et unions                                                                                             | D        | Non implémenté |

## Auteurs

**Hexanome 4234**
- Arthur Galet
- Alexandre Rosard
- Irvin Cote
- Youssef Sidhom
- Josue Venegas
- Abdel-Latif Hamdan
