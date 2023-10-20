#!/bin/bash

# Comprobar si se proporcionó un argumento
if [ $# -ne 1 ]; then
    echo "Uso: $0 <directorio>"
    exit 1
fi

directorio="$1"

# Comprobar si el directorio existe
if [ -d "$directorio" ]; then
    # Borrar el contenido del directorio
    rm -rf "$directorio"
fi

# Crear el directorio
mkdir -p "$directorio"

# Crear los archivos y enlaces
touch "$directorio/fichero1"
echo "0123456789" > "$directorio/fichero2"
ln -s "$directorio/fichero2" "$directorio/enlaceS"
ln "$directorio/fichero2" "$directorio/enlaceH"

# Mostrar atributos de los archivos
for archivo in "$directorio"/*; do
    echo "Atributos de $archivo:"
    stat "$archivo"
    echo
done

#1. Fichero1 ocupa 0 bloques y 0 bytes. Fichero2 8 bloques y 11 bytes.
#2. El tamaño es 8 bytes. Porque tenemos dos ficheros que apuntan al mismo 
#   i-nodo (ficheros2 y el enlace duro).
#3. Comparte i-nodos enlaceH con fichero2 por ser un hard link.
#4. Desde enlaceH sí, pero desde enlaceS no.
#5. Se modifica el momento de acceso, modificación y cambio. Podemos usar -a/-m
#   para modificar solo el momento de acceso/modificación.
