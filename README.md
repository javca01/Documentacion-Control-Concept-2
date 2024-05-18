# Documentación Interfaz Control Concept 2
La interfaz de control de la adaptación de la máquina de remo Concept 2, fue definida como un GUI modelada en Python usando la librería tktinker, en lo que sigue, se definirán las características que la integran con el fin de lograr un uso adecuado de esta y con ello conseguir una manipulación idónea de la máquina de remo Concept 2 adaptada. La aplicación desarrollada lleva por nombre **Control Concept 2**.

## Librerías empleadas
<p align="center"><img src="https://i.postimg.cc/Xvcf8RrP/1.png"></p>

Entre las librerías empleadas se encuentra **matplotlib**, la cual permite la representación gráfica de datos contenidos en arrays u otras estructuras de datos ya sea de forma estática o interactiva, de esta librería se usaron especialmente las clases **FigureCanvasTkAgg** y **Figure**, las cuales permiten la definición, ajuste y configuración de contenedores gráficos que alijan lienzos en los cuales se pueden representar un conjunto de datos.

Por otro lado, se empleó también la librería **tktinker**, la cual posibilita la gestión de objetos gráficos para la composición de Interfaces Gráficas de Usuario (GUI), que pueden derivar en el desarrollo de aplicaciones de escritorio multiplataforma.

Otra de las librerías empleadas fue **numpy**, la cual está dirigida a la ejecución de cálculos numéricos de forma ágil, por lo que suele emplearse con frecuencia en computación científica, incluye funciones de álgebra lineal, número aleatorios, transformada de Fourier, entre otros; brinda la posibilidad de manipular arreglos N-dimensionales, cuenta con funciones sofisticadas y permite la integración con C/C++ y Fortran.

El módulo **serial** permite el acceso al puerto serie proporcionan backeds para Python que pueden correr en Linux, OSX, Windows, IronPython, entre otros.

El módulo **time**, está dotado con funciones relacionadas con la base de tiempo del sistema para aplicaciones que requieran de sincronización con el reloj interno del sistema o de mediciones temporales sobre esta base.

## Definición de variables globales:
Las variables globales fueron definidas para acciones de control  y gestión de acciones que requerían de conocer el estado de variables que podían existir en varias funciones, algunas de estas variables son: **port_selected**, que permitía saber si se había elegido un puerto de la lista desplegable de la GUI, **data** es el array que contiene los datos que provienen del puerto y que posteriormente son representados en las gráficas, **Samples** que contiene la cantidad de muestras bajo las cuales se gestiona la actualización de las gráficas para dar un efecto de continuidad luego de ocupar las primeras muestras definidas en el eje horizontal de las gráficas y **Calibrated**, que es una variable booleana que almacena el estado actual del proceso de calibración, False si no se ha realizado y True si ya se realizó.

<p align="center"><img src="https://i.postimg.cc/Vv5Xz5Ky/2.png"></p>

## Definición de ventana principal:
En la función **interfaz_principal()**, se definen todas las propiedades de la ventana que se considera la interfaz principal, aquí se define el título principal, la paleta de colores de los objetos, la lista desplegable para la elección del puerto de comunicaciones, los botones de control y los canvas para la representación de los datos obtenidos por los sensores y que permiten definir la cinemática del sistema.

<p align="center"><img src="https://i.postimg.cc/44FzBb3X/3.png"></p>

<p align="center"><img src="https://i.postimg.cc/d1Vy9LP7/4.png"></p>

## Función no_clave():
La función **no_clave()**, es llamada cuando la contraseña ingresada en el Login es incorrecta, generando la respectiva ventana para indicarle al usuario la existencia de este error.

<p align="center"><img src="https://i.postimg.cc/kGZKG9H2/5.png"></p>

## Función borrar_interfaz_principal():
Esta función es llamada cuando el usuario cierra la ventana de la interfaz principal con el objetivo de destruir el objeto creado al emitir la ventana de la interfaz principal.

<p align="center"><img src="https://i.postimg.cc/1tvpxDcc/6.png"></p>

## Función borrar_no_clave():
Elimina o cierra la ventana que es generada cuando hay un error en la contraseña ingresada por el usuario.

<p align="center"><img src="https://i.postimg.cc/LX9z0xf8/7.png"></p>

## Función borrar_no_usuario():
Elimina o cierra la ventana que se genera cuando el usuario ha ingresado un valor incorrecto en el campo usuario.

<p align="center"><img src="https://i.postimg.cc/wBd5DqjJ/8.png"></p>

## Función btn_calibrar():
Esta función es llamada cuando se oprime el botón calibrar en la interfaz gráfica, al oprimir este botón la variable **estado_operacion** toma el valor de “Calibrando…”, cadena que aparece en la interfaz gráfica principal para indicarle al usuario que la máquina se encuentra en proceso de calibración, también se envía a la ESP32 el carácter a para indicarle que corra la rutina asociada al proceso de calibración.

<p align="center"><img src="https://i.postimg.cc/TYLnSLyg/9.png"></p>

## Función btn_iniciar():
Esta función es invocada cuando el botón Iniciar es oprimido en la interfaz gráfica, al entrar en funcionamiento la variable **estado_operacion** toma el valor de **“Iniciando operación”** el cual aparece en la interfaz principal para indicarle al usuario que la máquina está en operación desarrollando el perfil de movimiento impuesto, también, se envía el carácter **b** a la ESP32 para que se ejecute la rutina asociada a la condición de trabajo normal de la máquina.

<p align="center"><img src="https://i.postimg.cc/cJZYrFFB/10.png"></p>

## Función btn_detener():
Esta función es llamada al oprimir el botón **detener** en la interfaz principal, al entrar en funcionamiento, la variable **estado_operacion** acoge el valor de **“Deteniendo…”**, para indicarle al usuario que la máquina se irá al reposo, en este modo, se envía el carácter 99 para que la ESP32 ejecute la rutina de detención de la máquina.

<p align="center"><img src="https://i.postimg.cc/tCKFHqPf/11.png"></p>

## Función plot_data():
Esta función entra en funcionamiento una vez que la rutina de calibración comienza a ejecutarse, su trabajo, se centra en la adquisición de los datos de los sensores de la máquina provenientes del puerto COM y representarlos de forma dinámica en los gráficos dispuestos para tal fin, los cuales permiten apreciar la cinemática de la máquina en tiempo real.

<p align="center"><img src="https://i.postimg.cc/mDpFpBBm/12.png"></p>

<p align="center"><img src="https://i.postimg.cc/W10gNBw4/13.png"></p>

## Invocar la ventana inicial de la GUI:
La línea **ventana_inicio()** al final del código permite llamar como primera acción la ventana inicial de la interfaz.

<p align="center"><img src="https://i.postimg.cc/q7LKn2YV/14.png"></p>

## Credenciales de inicio:
Para el inicio de sesión en la aplicación es necesario que ingrese los siguientes datos; en el usuario deberá ingresar **admin** y en la contraseña, deberá introducir **unal2022**.

## Procedimiento de uso de la interfaz:
Para el uso de la aplicación **Control Concept 2** se debe hacer doble clic sobre ícono de la aplicación, posterior a esto surgirá la ventana inicial de la aplicación, la cual se muestra en la imagen de abajo.

<p align="center"><img src="https://i.postimg.cc/Rh3Jjy04/15.png"></p>

Al oprimir el botón **Ingresar** emergerá una ventana secundaria, que permitirá colocar las credenciales de acceso a la aplicación, las cuales se mencionaron en el apartado anterior. La imagen de la ventana de Login se muestra abajo.

<p align="center"><img src="https://i.postimg.cc/tRfxhqrT/16.png"></p>

Una vez que se han colocado en las respectivas casillas, las credenciales correctas, se debe oprimir el botón **Ingresar**. Esto nos llevará a la interfaz principal, la cual se muestra en la imagen que sigue.

<p align="center"><img src="https://i.postimg.cc/65vGckSh/17.png"></p>

La interfaz principal cuenta con una lista desplegable en la que se muestran todos los puertos COM disponibles, en el caso de que salga más de uno se recomienda ir al **Administrador de dispositivos** en Windows o el que haga las veces en otros sistemas operativos para observar el puerto asociado a la ESP32, una vez se confirme dicha información, se debe regresar a la interfaz principal y seleccionar el puerto correcto, luego se debe oprimir el botón **Seleccionar Puerto**, una vez hecho esto la interfaz ya estará plenamente disponible para su uso, es decir, estará en estado operativo. Al estar la interfaz principal operativa, se debe oprimir en primer lugar el botón **Calibrar**, esto hará que la silla de la máquina se desplace de extremo a extremo a velocidad constante, midiendo la distancia entre los sensores ReedSwitch que sirven de limitadores del recorrido.

<p align="center"><img src="https://i.postimg.cc/QCB03Hnm/interfaz-3.png"></p>

Posteriormente se podrá oprimir el botón **Iniciar**, no antes, con lo cual la máquina desarrollará el perfil de movimiento impuesto mostrando los datos de posición, velocidad y aceleración de la silla en las gráficas. Finalmente, el botón **Detener**, podrá oprimirse siempre que la máquina se encuentre en movimiento, es decir, ya sea que esté ejecutando la rutina de calibración o se encuentre desarrollando el perfil de movimiento en condiciones de trabajo normal, cabe destacar que la rutina de detención contempla que la silla se detenga y posteriormente se desplace a la posición de arranque, inicial o de home para prepararse para una siguiente activación.

## Script para la ESP32:
En este Script se definen todas las funciones de adquisición de datos de los sensores y se define la gestión de puertos para emitir la señal PWM para gobernar el Driver de potencia que permite accionar el motor de la máquina Concept 2, en este también se definen el conjunto de instrucciones que permiten realizar la rutina de calibración, considerando las señales de los sensores ReedSwitch, los cual son una referencia posicional que definen los extremos longitudinales de la máquina para ejecutar los movimientos de la máquina, de igual, forma se definen las rutinas de trabajo, detención y home, en función de los datos recibidos desde la interfaz.

Se definen las librerías a utilizar, la **VL53L0X** permite la gestión de los valores capturados por el sensor de distancia, por otro lado, la librería **Wire** posibilita la comunicación con dispositivos que requieren del protocolo I2C. 

<p align="center"><img src="https://i.postimg.cc/mk01RjcM/18.png"></p>

Posteriormente se definen algunas variables globales para el control de flujo de las funciones definidas en el script, algunas de estas son variables booleanas que permiten usarlas como flags para el control de activación de ciertas funciones, otras permiten definir los pines de los diferentes sensores o permiten la contabilización y registro de algún parámetro de interés dentro del proceso.

<p align="center"><img src="https://i.postimg.cc/JzCH2TCC/19.png"></p>

Se definen las variables de salida para la señal PWM que controla el driver de potencia asociado al motor, así como aquellas variables de entrada para el registro de los eventos relacionados con los reedswitch instalados en la máquina.

<p align="center"><img src="https://i.postimg.cc/t4dnQh9Z/20.png"></p>

Se establecen los parámetros característicos del canal de emisión de la señal PWM.

<p align="center"><img src="https://i.postimg.cc/TPRLRpPk/21.png"></p>

Posteriormente se declara, el vector que contiene los diferentes valores del ciclo PWM que permite reproducir el perfil de movimiento de trabajo en la máquina.

<p align="center"><img src="https://i.postimg.cc/HLBJjdcT/22.png"></p>

Declaración de los flags para el control de activación de las diferentes funciones del programa.

<p align="center"><img src="https://i.postimg.cc/fLPJjCfF/23.png"></p>

Las siguientes variables son empleadas para determinar los deltas de tiempo, necesarios para el cálculo de la aceleración.

<p align="center"><img src="https://i.postimg.cc/1tX8q2ww/24.png"></p>

En el **setup** se define la configuración de los puertos, así como las interrupciones por señales externas que se vayan a emplear dentro del programa, de igual forma se inicializan los sensores.

<p align="center"><img src="https://i.postimg.cc/LhK5Gs73/25.png"></p>

<p align="center"><img src="https://i.postimg.cc/hv7hCmCG/26.png"></p>

En el **loop()** se escriben las líneas que constituirán el programa principal, el cual se estará ejecutando permanentemente. Aquí se coloca toda la lógica del programa, se indican las acciones a ejecutar cuando la ESP32 recibe el carácter **a**, el cual está asociado a la rutina de calibración, cuando recibe el carácter **b**, el cual representa el inicio de la rutina de trabajo, o cuando recibe el carácter **c**, el cual representa la rutina de detención de la máquina. 

<p align="center"><img src="https://i.postimg.cc/ryv9qN2Q/27.png"></p>
<p align="center"><img src="https://i.postimg.cc/8kFH8n97/28.png"></p>
<p align="center"><img src="https://i.postimg.cc/pXzJr52x/29.png"></p>
<p align="center"><img src="https://i.postimg.cc/2SYFZj6W/30.png"></p>
<p align="center"><img src="https://i.postimg.cc/RZT1Qr6D/31.png"></p>
<p align="center"><img src="https://i.postimg.cc/3wJXtL2J/32.png"></p>
<p align="center"><img src="https://i.postimg.cc/7LsgdgN1/33.png"></p>
<p align="center"><img src="https://i.postimg.cc/d1h8YLWb/34.png"></p>
<p align="center"><img src="https://i.postimg.cc/MTg1KqNv/35.png"></p>
<p align="center"><img src="https://i.postimg.cc/8cWM7xgw/36.png"></p>

## Función rutina_movimiento_cw():
Esta función permite cambiar los estados de los flags para que el programa principal implemente un movimiento del eje del motor en sentido horario.

<p align="center"><img src="https://i.postimg.cc/44hz2HHt/37.png"></p>

## Función rutina_movimiento_ccw():
La función permite ajustar los estados de los flagas para permitir que el programa principal se implemente un movimiento del eje del motor en sentido antihorario.

<p align="center"><img src="https://i.postimg.cc/02R7T574/38.png"></p>

## Función doEncodeA():
En este bloque se definen las instrucciones para llevar una contabilización en un parámetro de control cuando los flancos altos de los canales A y B del encoder coinciden.

<p align="center"><img src="https://i.postimg.cc/PrW1TG45/39.png"></p>

## Función doEncodeB():
Permite afectar una variable contadora cuando el flanco alto y bajo de las señales de los canales A y B del encoder, coincide.

<p align="center"><img src="https://i.postimg.cc/P5dYq63z/40.png"></p>

Las dos funciones anteriores (doEncodeX) permiten determinar el sentido de giro del eje del motor y hacer una contabilización de los pulsos como medida de la velocidad angular del motor.

## Función lectura_distancia():
En esta función se escala la medida obtenida por el sensor de distancia a cm.

<p align="center"><img src="https://i.postimg.cc/XYxFBwtM/41.png"></p>

## Función lectura_encoder():
En este bloque se determina la velocidad angular del eje del motor en función de los datos obtenidos del encoder, los cuales son convertidos a rpm.

<p align="center"><img src="https://i.postimg.cc/VN6tqJcJ/42.png"></p>

## Función detener():
Permite llevar la máquina al reposo e invoca la rutina de home.

<p align="center"><img src="https://i.postimg.cc/zfcg0qJ0/43.png"></p>

## Función rutina_home():
Este bloque de instrucciones permite llevar la silla a la posición inicial, sin importar el lugar de la trayectoria donde el usuario haya decidido oprimir el botón **detener**.

<p align="center"><img src="https://i.postimg.cc/Pxv870Jh/44.png"></p>

<p align="center"><img src="https://i.postimg.cc/fy40Q5Sd/45.png"></p>

## Enlace de descarga de la aplicación:

<p align="center"><img src="https://i.postimg.cc/PqKkQgtm/concept-2.png"></p>

El ejecutable de la aplicación puede ser descargado desde el enlace que se muestra abajo:

Clic [aquí](https://bit.ly/44jrLcn) para descargar la aplicación Control Concept 2.

También puedes acceder a la aplicación a través de este otro [enlace](https://bit.ly/3ymeiF3).

## Bibliografía:
* Cardellino, F. (2021). La guía definitiva del paquete NumPy para computación científica en Python. Recuperado de: https://www.freecodecamp.org/espanol/news/la-guia-definitiva-del-paquete-numpy-para-computacion-cientifica-en-python/

* Redacción KeepCoding. (2023). ¿Qué es Tkinter?. Recuperado de: https://keepcoding.io/blog/que-es-tkinter/ 

* Hunter, J., Dale D., Firing, E., Droettboom, M. (2002-2012). Matplotlib developmet team. Recuperado de: https://matplotlib.org/stable/api/figure_api.html 

* CodersLegacy. (N.R). FigureCanvasTkAgg with Matplotlib and Tkinter. Recuperado de: https://coderslegacy.com/figurecanvastkagg-matplotlib-tkinter/ 

* Liechti, C. (2001-2020). Welcome to pySerial’s documentation. Recuperado de: https://pyserial.readthedocs.io/en/latest/ 

* Python Software Foundation. (2001-2023). Acceso a tiempo y conversiones. Recuperado de: https://docs.python.org/es/3/library/time.html 














