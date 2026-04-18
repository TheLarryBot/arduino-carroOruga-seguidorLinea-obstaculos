```markdown
# 🤖 Seguidor de Línea + Evasión de Obstáculos
**Arduino Nano · L298N · HC-SR04 · 2× TCRT5000**

Robot de dos ruedas que sigue una línea negra sobre superficie blanca y evade obstáculos de forma autónoma mediante una maniobra temporizada, reincorporándose a la línea automáticamente.

---

## 📋 Tabla de Contenido

- [Características](#características)
- [Hardware](#hardware)
- [Conexiones](#conexiones)
- [Configuración](#configuración)
- [Funcionamiento](#funcionamiento)
- [Secuencia de Evasión](#secuencia-de-evasión)
- [Puesta en Marcha](#puesta-en-marcha)
- [Monitor Serial](#monitor-serial)
- [Limitaciones Conocidas](#limitaciones-conocidas)

---

## Características

- ✅ Seguimiento de línea en tiempo real con dos sensores IR (TCRT5000)
- ✅ Detección de obstáculos con sensor ultrasónico (HC-SR04)
- ✅ Maniobra de evasión totalmente autónoma
- ✅ Reincorporación automática a la línea tras la evasión
- ✅ Salida de depuración por Serial a 9600 baudios

---

## Hardware

| Componente           | Cant. | Notas                              |
|----------------------|-------|------------------------------------|
| Arduino Nano         | 1     | Cualquier Nano AVR de 5 V          |
| Driver L298N         | 1     | Puente H doble                     |
| Motores DC con caja  | 2     | Uno por lado                       |
| HC-SR04              | 1     | Sensor ultrasónico de distancia    |
| TCRT5000             | 2     | Sensores IR reflectivos de línea   |
| Fuente de poder      | 1     | 7–12 V para motores · 5 V para Nano|
| Chasis + ruedas      | 1     | Tracción diferencial de dos ruedas |

---

## Conexiones

### Ultrasónico — HC-SR04

| HC-SR04 | Arduino Nano |
|---------|-------------|
| VCC     | 5 V         |
| GND     | GND         |
| TRIG    | D2          |
| ECHO    | D3          |

### Driver de motores — L298N

| L298N | Arduino Nano |
|-------|-------------|
| ENA   | D5 (PWM)    |
| IN1   | D7          |
| IN2   | D8          |
| IN3   | D9          |
| IN4   | D10         |
| ENB   | D6 (PWM)    |
| GND   | GND         |

> ⚠️ Conectar la fuente de los motores (7–12 V) al pin **VS** del L298N,
> **nunca** al VIN del Arduino.

### Sensores IR — TCRT5000

| Sensor        | Arduino Nano |
|---------------|-------------|
| Salida Izq.   | D11         |
| Salida Der.   | D12         |
| VCC           | 5 V         |
| GND           | GND         |

> Los sensores entregan `LOW` sobre línea negra y `HIGH` sobre blanco.
> Ajustar el potenciómetro de cada módulo hasta que el LED indicador
> conmute limpiamente al cruzar el borde de la línea.

---

## Configuración

Todos los parámetros ajustables están definidos como constantes `constexpr`
al inicio del sketch, sin números mágicos dispersos en el código.

```cpp
constexpr uint8_t VELOCIDAD        = 190;  // PWM 0–255. Usar ~70 con fuente de 12 V y 190 con 2 baterias de 3.7 V.
constexpr uint8_t DISTANCIA_LIMITE = 20;   // Umbral de detección en cm.
```

| Constante          | Valor por defecto | Descripción                                      |
|--------------------|-------------------|--------------------------------------------------|
| `VELOCIDAD`        | `190`             | Ciclo de trabajo PWM para ambos motores          |
| `DISTANCIA_LIMITE` | `20`              | Distancia (cm) que dispara la evasión            |
| `DISTANCIA_MAXIMA` | `999`             | Valor centinela cuando no se recibe eco          |

---

## Funcionamiento

```
loop()
 ├─ medirDistancia()              ← pulsa el HC-SR04, retorna cm
 │
 ├─ distancia ≤ DISTANCIA_LIMITE  Y  !modoEvasion
 │    └─ evadirObstaculo()        ← secuencia temporizada bloqueante
 │
 └─ !modoEvasion
      └─ seguirLinea()            ← lee sensores IR y corrige dirección
```

### Lógica de Seguimiento

| Sensor Izq. | Sensor Der. | Acción                          |
|-------------|-------------|----------------------------------|
| LOW         | LOW         | Avanzar (centrado en la línea)  |
| LOW         | HIGH        | Girar izquierda (deriva derecha)|
| HIGH        | LOW         | Girar derecha (deriva izquierda)|
| HIGH        | HIGH        | Detenerse (línea perdida)       |

---

## Secuencia de Evasión

Al detectar un obstáculo a menos de `DISTANCIA_LIMITE` cm, el robot
ejecuta la siguiente maniobra de lazo abierto (Estos valores son modificables):

```
1.  Detener      — pausa breve antes de maniobrar
2.  Girar der.   — 400 ms, esquiva el obstáculo lateralmente
3.  Detener      — estabilizar
4.  Avanzar      — 1000 ms, supera el largo del obstáculo
5.  Detener      — estabilizar
6.  Girar izq.   — 400 ms, reorienta hacia la línea
7.  Detener      — estabilizar
8.  Avanzar      — 1000 ms, se acerca a la línea
9.  Detener      — estabilizar
10. Girar izq.   — barrido hasta que un sensor IR detecte la línea
11. Girar der.   — 200 ms de corrección para centrarse en la línea
12. Reanudar seguimiento normal
```

> **Consejo:** Si el robot constantemente se pasa o se queda corto al
> reincorporarse, ajustar los valores de `delay()` en los pasos 2, 4,
> 6 y 8 según la geometría del chasis y la velocidad de los motores.

---

## Puesta en Marcha

1. Clonar o descargar este repositorio.
2. Abrir `line_follower.ino` en **Arduino IDE ≥ 1.8** o **Arduino IDE 2**.
3. Seleccionar **Placa → Arduino Nano** y el **Puerto** correspondiente.
4. Ajustar `VELOCIDAD` y `DISTANCIA_LIMITE` según el hardware.
5. Cargar el sketch.
6. Abrir el Monitor Serial a **9600 baudios**.
7. Colocar el robot sobre la línea y energizar los motores.

---

## Monitor Serial

| Mensaje               | Significado                                           |
|-----------------------|-------------------------------------------------------|
| `OBSTACULO DETECTADO` | Obstáculo dentro del umbral — iniciando evasión       |
| `LINEA REENCONTRADA`  | Sensor IR detectó la línea — reanudando seguimiento   |

---

## Limitaciones Conocidas

- **Evasión de lazo abierto** — los retardos son fijos; el resultado varía
  con el voltaje de la batería y la fricción de la superficie. A menor
  voltaje, los giros serán más cortos de lo esperado.
- **Obstáculo único** — el robot no maneja obstáculos detectados
  *durante* la maniobra de evasión.
- **`pulseIn` bloqueante** — la medición de distancia detiene la ejecución
  hasta 30 ms por llamada. Para mayor respuesta, considerar `NewPing`
  o interrupciones por temporizador.
- **Sin PID** — el seguimiento de línea usa control todo-o-nada (bang-bang).
  Para trayectorias más suaves a mayor velocidad, se recomienda un
  controlador PID.
```



