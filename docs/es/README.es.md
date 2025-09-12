## Tesis de Licenciatura titulada: <br> <br> «Modelado e implementación de ataques en redes de sensores inalámbricos»

<br> <br>

### Resumen:

Esta tesis estudia el funcionamiento del protocolo de enrutamiento AODV, que se aplica principalmente a redes móviles ad hoc (MANET) y otras redes inalámbricas ad hoc. En el contexto del estudio, se investiga la robustez de este protocolo frente a ataques maliciosos, enfocándose en particular en los ataques tipo blackhole, mientras se proponen medidas de mitigación para reducir el impacto de dichos ataques.

Para el análisis del protocolo, se utilizó el simulador de redes NS-3 para crear una topología de red compuesta por 15 nodos, de los cuales 4 son móviles, y se colocaron fuentes de energía en los nodos de interés. La evaluación del desempeño del protocolo se basa en el examen de tres métricas clave: pérdida de paquetes, rendimiento (throughput) y consumo de energía de los nodos.

Los resultados del estudio contribuyen a comprender el comportamiento del protocolo bajo ataque y al desarrollo de infraestructuras de red más resilientes para redes inalámbricas ad hoc.

<br> <br>

### Estructura del Repositorio

En este repositorio, encontrará lo siguiente en cada carpeta:

- **_[aodv-modified-sources](../../aodv-modified-sources/)_** → Los archivos fuente modificados del protocolo de enrutamiento AODV incluidos en la instalación de NS-3.
- **_[ns3-simulations](../../ns3-simulations/)_** → Esta carpeta contiene el código principal de la red y el script para ejecutar los escenarios.
- ***[diagrams](../../diagrams/)*** -> Todos los diagramas de [secuencia](/diagrams/puml-diagrams), [consumo de energía](/diagrams/energy-consumption-diagrams) y [métricas de red](/diagrams/network-metrics-diagrams) que aparecen en el informe del proyecto. Se incluyen tanto las imágenes finales generadas como los archivos ejecutables.



El archivo - **_[user-manual.es.md](user-manual.es.md)_** contiene instrucciones de instalación y uso.

La **[tesis](../../CS_IoannisArgyropoulos_2025_Thesis.pdf)** completa y su **[presentación](../../thesis_presentation.pdf)** correspondiente están disponibles para su consulta.

<br> <br>

Puede ver una demostración de todos los escenarios en [asciicast](https://asciinema.org/a/Dx3fkB8rOoSvbCGM9SPrRhySV): <br>

[![asciicast](https://asciinema.org/a/Dx3fkB8rOoSvbCGM9SPrRhySV.svg)](https://asciinema.org/a/Dx3fkB8rOoSvbCGM9SPrRhySV)
