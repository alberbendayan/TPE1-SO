# TPE_SO

Repositorio para el **TPE** de la materia **Sistemas Operativos** en el **ITBA**.

## Tabla de contenidos

- [TPE\SO](#tpe_arqui)
  - [Tabla de contenidos](#tabla-de-contenidos)
  - [Requisitos](#requisitos)
  - [Instalacion](#instalacion)
    - [Docker](#docker)
  - [Ejecutando](#ejecutando)
  - [Autores](#autores)

## Requisitos

Para la realizacion del trabajo se requieren los siguientes elementos:

- **docker:** Nos va a permitir compilar el TPE dentro de un contenedor


## Instalacion

### Docker

La opcion mas facil va a ser ir a la pagina oficial de [docker](https://www.docker.com/products/docker-desktop/) donde podran elegir la version a instalar.

Luego hay que utilizar el contenedor brindado por la catedra **agodio/itba-so:2.0**

## Ejecutando


Cumpliendo todos los requisitos, los pasos para la ejecucion son los siguientes:

```sh
$> make 
```

Generara un set de binarios ejecutables

- *app*: Recibe uno o mas argumentos por entrada estandar, crea una cantidad suficiente de procesos hijos para calcular su md5. Devuelve el resultado de dos maneras distintas:
    - *'result.txt'*: archivo de texto con la salida
    - *'shared memory'*: deja el resultado en una memoria compartida '/my_shm' la cual puede ser utilizada por la view.
- *child*: Son los procesos esclavos los cuales reciben por entrada estandar (mediante pipes) el nombre del archivo y retorna por salida estandar el hash md5 junto al pid del proceso y el nombre del archivo.
- *view*: Recibe el nombre de la shared memory e imprime lo que hay en la misma, las formas de recibir el parametro son:
    - Entrada estandar
    - Argumento

### Formas de ejecutar

```sh
$> ./app [FILES...]
```
> Deja creada la shared memory y el archivo de texto *'result.txt'*

```sh
$> ./app [FILES...] | ./view
```
> Ejecuta todo e imprime en pantalla

```sh
$> ./child
```
> Ejecuta el child y espera un filename por entrada estandar

```sh
$> ./app
$> ./view /my_shm
```
> Ejecuta primero la app y luego ejecuta la view pasandole por parametro la shared memory







## Autores

- **_`Bendayan, Alberto`_** - **_`Legajo: 62786`_**
- **_`Boullosa, Juan Cruz`_** - **_`Legajo: 63414`_**
- **_`Ves Losada, Tobias`_** - **_`Legajo: 63342`_**
