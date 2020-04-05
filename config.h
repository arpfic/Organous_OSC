/* -----------------------------------------------------------------------------
 * MAIN CONFIG : changer ici les réglages principaux
 */
#define IF_VER                                  "v0.1"
#define IF_NAME                                 "SURAIGU mod."

// b-side présent ?
#define B_SIDE                                  0
// Commencer à compter à 1 ? No effect
#define HUMAN_OFFSET                            0

/* -----------------------------------------------------------------------------
 * LEDS
 */
#define LED_GREEN                               LED1
#define LED_BLUE                                LED2
#define LED_RED                                 LED3

/* -----------------------------------------------------------------------------
 * COIL DEFAULTS SET'S
 * - attack delay before sustain (millisec)
 * - attack value  (0.0-1.0)
 * - sustain value (0.0-1.0)
 */
#define COIL_ATTACK_DELAY                       20
#define COIL_ATTACK                             (float)1.0
#define COIL_SUSTAIN                            (float)0.5

/* -----------------------------------------------------------------------------
 * OSC
 */
#define OSC_CLIENT_PORT                         12345
#define DEBUG_PORT                              12346
#define MAX_PQT_LENGTH                          1024

/* -----------------------------------------------------------------------------
 * DRIVER ENABLE TABLE
 */
#define ENABLE_PINS                             24
/* a-side (right) : OUT 1-24 
 */
#define DRV_EN1                                 PC_10
#define DRV_EN2                                 PC_12
#define DRV_EN3                                 PD_2
#define DRV_EN4                                 PF_6
#define DRV_EN5                                 PF_7
#define DRV_EN6                                 PA_15
#define DRV_EN7                                 PA_0
#define DRV_EN8                                 PE_4
#define DRV_EN9                                 PC_2
#define DRV_EN10                                PC_0
#define DRV_EN11                                PC_3
#define DRV_EN12                                PD_3
#define DRV_EN13                                PD_4
#define DRV_EN14                                PG_3
#define DRV_EN15                                PD_5
#define DRV_EN16                                PD_6
#define DRV_EN17                                PE_3
#define DRV_EN18                                PF_2
#define DRV_EN19                                PG_1
#define DRV_EN20                                PD_0
#define DRV_EN21                                PG_0
#define DRV_EN22                                PE_1
#define DRV_EN23                                PG_15
#define DRV_EN24                                PG_9

#define PCA_A_SCL                               PF_1
#define PCA_A_SDA                               PF_0
#define PCA_A_OE                                PE_5
#define PCA_A_RST                               PD_1

#define DRV_A_RST                               PG_12
#define DRV_A_FAULT                             PC_11

/* b-side (left) : OUT 25-48 
 */
#define DRV_EN25                                PC_8
#define DRV_EN26                                PC_6
#define DRV_EN27                                PB_8
#define DRV_EN28                                PB_9
#define DRV_EN29                                PA_5
#define DRV_EN30                                PB_12
#define DRV_EN31                                PB_11
#define DRV_EN32                                PB_6
#define DRV_EN33                                PC_7
#define DRV_EN34                                PB_2
#define DRV_EN35                                PB_1
#define DRV_EN36                                PB_10
#define DRV_EN37                                PD_15
#define DRV_EN38                                PE_15
#define DRV_EN39                                PE_13
#define DRV_EN40                                PE_11
#define DRV_EN41                                PF_13
#define DRV_EN42                                PF_3
#define DRV_EN43                                PF_12
#define DRV_EN44                                PG_14
#define DRV_EN45                                PF_11
#define DRV_EN46                                PE_0
#define DRV_EN47                                PD_10
#define DRV_EN48                                PG_8

#define PCA_B_SCL                               PF_14
#define PCA_B_SDA                               PF_15
#define PCA_B_OE                                PD_14
#define PCA_B_RST                               PE_9

#define DRV_B_RST                               PG_5
#define DRV_B_FAULT                             PC_9

/* -----------------------------------------------------------------------------
 * MISC
 */
#define DEFAULT_I2C_TAG                         0xD2