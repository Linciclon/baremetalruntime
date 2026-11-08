
#include "bsp_io.h"
#include "uart_api.h"


/* Key code for writing PRCR register. */
#define BSP_IO_PRV_PRCR_KEY        (0xA500U)
#define BSP_IO_REG_PROTECT_GPIO    (0x0004U)

typedef struct st_ioport_cfg_data
{
    uint32_t p_reg    : 1;
    uint32_t pm_reg   : 2;
    uint32_t pmc_reg  : 1;
    uint32_t pfc_reg  : 4;
    uint32_t drct_reg : 6;
    uint32_t rsel_reg : 1;
    uint32_t reserved : 17;
} ioport_cfg_data_t;

#define IOPORT_PRV_PORT_BITS       (0xFF00U)
#define IOPORT_PRV_PIN_BITS        (0x00FFU)
#define IOPORT_PRV_PORT_OFFSET     (8U)

/* Added definitions */
#define IOPORT_PIN_NUM_MUX         (8U)
#define IOPORT_REGION_SEL_SAFE     (0U)
#define IOPORT_REGION_SEL_NSAFE    (1U)
#define IOPORT_RSEL_MASK           (0x01U)
#define IOPORT_PM_BIT_MASK         (0x0003U)
#define IOPORT_PFC_BIT_MASK        (0x0000000FU)
#define IOPORT_DRTCL_BIT_MASK      (0x000000FFU)
#define IOPORT_ELC_PEL_MASK        (0x80)
#define IOOPRT_ELC_PGC_MASK        (0x88)
#define IOPORT_ELC_PEL_PSM_HIGH    (0x20)


#define IOPORT_CODE_VERSION_MAJOR    (1U)
#define IOPORT_CODE_VERSION_MINOR    (0U)

#define IOPORT_SINGLE_PORT_NUM       (4)
#define IOPORT_PORT_GROUP_NUM        (2)
#define IOPORT_PORT_GROUP_1          (0)
#define IOPORT_PORT_GROUP_2          (1)
#define IOPORT_SINGLE_PORT_0         (0)
#define IOPORT_SINGLE_PORT_1         (1)
#define IOPORT_SINGLE_PORT_2         (2)
#define IOPORT_SINGLE_PORT_3         (3)

/* Switch IOPORT register region either safety or non safety */
#define IOPORT_PRV_PORT_ADDRESS(region_sel)    (region_sel == 1 ? (R_PORT_NSR) : (R_PORT_SR))

typedef uint16_t ioport_size_t;  

typedef enum e_ioport_event_output_operation
{
    IOPORT_EVENT_OUTPUT_OPERATION_LOW    = 0x0, ///< Set Low output to output operation
    IOPORT_EVENT_OUTPUT_OPERATION_HIGH   = 0x1, ///< Set High output to output operation
    IOPORT_EVENT_OUTPUT_OPERATION_TOGGLE = 0x2, ///< Set toggle output to output operation
    IOPORT_EVENT_OUTPUT_OPERATION_BUFFER = 0x3, ///< Set buffer value output to output operation
} ioport_event_output_operation_t;

typedef struct st_ioport_event_group_output
{
    uint8_t pin_select;                        ///< Port number specified to output port group
    ioport_event_output_operation_t operation; ///< Port group operation select
} ioport_event_group_output_t;

typedef enum e_ioport_event_control
{
    IOPORT_EVENT_CONTROL_DISABLE = 0x0, ///< Disable function related with event link
    IOPORT_EVENT_CONTROL_ENABLE  = 0x1, ///< Enable function related with event link
} ioport_event_control_t;

typedef enum e_ioport_event_detection
{
    IOPORT_EVENT_DETECTION_RISING_EDGE  = 0x0, ///< Set rising edge to event detection for input event
    IOPORT_EVENT_DETECTION_FALLING_EDGE = 0x1, ///< Set falling edge to event detection for input event
    IOPORT_EVENT_DETECTION_BOTH_EGDE    = 0x2, ///< Set both edges to event detection for input event
} ioport_event_detection_t;

typedef struct st_ioport_event_group_input
{
    ioport_event_control_t   event_control;     ///< Event link control for input port group
    ioport_event_detection_t edge_detection;    ///< Edge detection select
    ioport_event_control_t   overwrite_control; ///< Buffer register overwrite control
    uint8_t pin_select;                         ///< Port number specified to input port group
    uint8_t buffer_init_value;                  ///< Buffer register initial value
} ioport_event_group_input_t;

typedef enum e_ioport_event_direction
{
    IOPORT_EVENT_DIRECTION_OUTPUT = 0x0, ///< Set output direction to single port
    IOPORT_EVENT_DIRECTION_INPUT  = 0x1, ///< Set input direction to single port
} ioport_event_direction_t;

typedef struct st_ioport_event_single
{
    ioport_event_control_t          event_control;  ///< Event link control for single port
    ioport_event_direction_t        direction;      ///< Event direction for single port
    uint16_t                        port_num;       ///< Port number specified to single port
    ioport_event_output_operation_t operation;      ///< Single port operation select
    ioport_event_detection_t        edge_detection; ///< Edge detection select
} ioport_event_single_t;

typedef struct  st_ioport_extend_cfg
{
    ioport_event_group_output_t port_group_output_cfg[IOPORT_PORT_GROUP_NUM]; ///< Output port group configuration
    ioport_event_group_input_t  port_group_input_cfg[IOPORT_PORT_GROUP_NUM];  ///< Input port group configuration
    ioport_event_single_t       single_port_cfg[IOPORT_SINGLE_PORT_NUM];      ///< Single input port configuration
} ioport_extend_cfg_t;



uint32_t g_protect_port_counter;




void enable_pin_access (void)
{
    /** Get the current state of interrupts */
    FSP_CRITICAL_SECTION_DEFINE;
    FSP_CRITICAL_SECTION_ENTER;

    /** If this is first entry then allow writing of PFS. */
    if (0 == g_protect_port_counter)
    {
        /** Disable protection using PRCR register. */

        /** When writing to the PRCR register the upper 8-bits must be the correct key. Set lower bits to 0 to
         * disable writes. */
        R_RWP_NS->PRCRN = ((R_RWP_NS->PRCRN | BSP_IO_PRV_PRCR_KEY) | BSP_IO_REG_PROTECT_GPIO);
        R_RWP_S->PRCRS  = ((R_RWP_S->PRCRS | BSP_IO_PRV_PRCR_KEY) | BSP_IO_REG_PROTECT_GPIO);
    }

    /** Increment the protect counter */
    g_protect_port_counter++;

    /** Restore the interrupt state */
    FSP_CRITICAL_SECTION_EXIT;
}

void disable_pin_access (void)
{

    /** Get the current state of interrupts */
    FSP_CRITICAL_SECTION_DEFINE;
    FSP_CRITICAL_SECTION_ENTER;

    /** Is it safe to disable PFS register? */
    if (0 != g_protect_port_counter)
    {
        /* Decrement the protect counter */
        g_protect_port_counter--;
    }

    /** Is it safe to disable writing of PFS? */
    if (0 == g_protect_port_counter)
    {
        /** Enable protection using PRCR register. */

        /** When writing to the PRCR register the upper 8-bits must be the correct key. Set lower bits to 0 to
         * disable writes. */
        R_RWP_NS->PRCRN = ((R_RWP_NS->PRCRN | BSP_IO_PRV_PRCR_KEY) & (uint16_t) (~BSP_IO_REG_PROTECT_GPIO));
        R_RWP_S->PRCRS  = ((R_RWP_S->PRCRS | BSP_IO_PRV_PRCR_KEY) & (uint16_t) (~BSP_IO_REG_PROTECT_GPIO));
    }

    /** Restore the interrupt state */
    FSP_CRITICAL_SECTION_EXIT;
}

static void ioport_event_config (const ioport_extend_cfg_t * p_extend_cfg_data)
{
    uint8_t               event_num;
    uint8_t               temp_value    = 0x00;
    uint8_t               single_enable = 0x00;
    uint8_t               group_enable  = 0x00;
    R_PORT_COMMON_Type  * p_ioport_regs;
    ioport_extend_cfg_t * ex_cfg;

    ex_cfg = (ioport_extend_cfg_t *) p_extend_cfg_data;

    enable_pin_access();           // Unlock Register Write Protection

    /* Get register address */
    p_ioport_regs = IOPORT_PRV_PORT_ADDRESS(IOPORT_REGION_SEL_NSAFE);

    /* Single port configuration */
    for (event_num = 0U; event_num < IOPORT_SINGLE_PORT_NUM; event_num++)
    {
        uint8_t port =
            (uint8_t) ((ex_cfg->single_port_cfg[event_num].port_num & IOPORT_PRV_PORT_BITS) >> IOPORT_PRV_PORT_OFFSET);
        uint8_t pin_num = (uint8_t) ex_cfg->single_port_cfg[event_num].port_num & IOPORT_PRV_PIN_BITS;

        temp_value = p_ioport_regs->ELC_PEL[event_num] & IOPORT_ELC_PEL_MASK;

        /* Port selection */
        if ((BSP_IO_PORT_16 >> IOPORT_PRV_PORT_OFFSET) == port)
        {
            temp_value |= 1U << 3;
        }
        else if ((BSP_IO_PORT_18 >> IOPORT_PRV_PORT_OFFSET) == port)
        {
            temp_value |= 1U << 4;
        }
        else
        {
            /* Do Nothing */
        }

        temp_value |= pin_num;         // Pin number setting

        /* When the pin specified as single input port, Set edge detection */
        /* When the pin specified as single output port, Set output operation */
        if (IOPORT_EVENT_DIRECTION_INPUT == ex_cfg->single_port_cfg[event_num].direction)
        {
            temp_value |= (uint8_t) (ex_cfg->single_port_cfg[event_num].edge_detection << 5); // Edge detection

            /* Edge detection enable */
            p_ioport_regs->ELC_DPTC |= (uint8_t) (1U << event_num);
        }
        else
        {
            temp_value |= (uint8_t) (ex_cfg->single_port_cfg[event_num].operation << 5); // Output operation
        }

        /* Set to ELC port setting register */
        p_ioport_regs->ELC_PEL[event_num] = temp_value;

        /* Single port event link function enable */
        if (IOPORT_EVENT_CONTROL_ENABLE == ex_cfg->single_port_cfg[event_num].event_control)
        {
            single_enable |= (uint8_t) (1U << event_num);
        }
    }

    /* Port group configuration */
    for (event_num = 0U; event_num < IOPORT_PORT_GROUP_NUM; event_num++)
    {
        /* Pin selection */
        uint8_t group_pin = ex_cfg->port_group_input_cfg[event_num].pin_select |
                            ex_cfg->port_group_output_cfg[event_num].pin_select;
        p_ioport_regs->ELC_PGR[event_num] = group_pin;

        if (IOPORT_EVENT_CONTROL_ENABLE == ex_cfg->port_group_input_cfg[event_num].event_control)
        {
            /* Input port group control */
            temp_value  = p_ioport_regs->ELC_PGC[event_num] & IOOPRT_ELC_PGC_MASK;
            temp_value |= ex_cfg->port_group_input_cfg[event_num].edge_detection;                      // Edge detection
            temp_value |= (uint8_t) (ex_cfg->port_group_input_cfg[event_num].overwrite_control << 2U); // Overwrite setting

            /* Buffer register initialization */
            p_ioport_regs->ELC_PDBF[event_num].BY = ex_cfg->port_group_input_cfg[event_num].buffer_init_value;

            /* Input port group event link function enable */
            group_enable |= (uint8_t) (1U << event_num);
        }

        /* Output port group operation */
        temp_value |= (uint8_t) (ex_cfg->port_group_output_cfg[event_num].operation << 4);

        /* Set to port group control register */
        p_ioport_regs->ELC_PGC[event_num] = temp_value;
    }

    /* Set to ELC port event control register */
    p_ioport_regs->ELC_ELSR2 = (uint8_t) ((single_enable << 4) | (group_enable << 2));

    disable_pin_access();          // Lock Register Write Protection
}




static void pin_set (bsp_io_port_pin_t pin, ioport_cfg_data_t * p_cfg_data)
{
    R_PORT_COMMON_Type * p_ioport_regs;
    uint32_t             temp_value;

    /* Get port and pin number */
    uint32_t port    = (IOPORT_PRV_PORT_BITS & (ioport_size_t) pin) >> IOPORT_PRV_PORT_OFFSET;
    uint32_t pin_num = (IOPORT_PRV_PIN_BITS & (ioport_size_t) pin);

    /* Setting for Safety region or Non safety region */
    if (p_cfg_data->rsel_reg == 1U)    // Setting for Non safety region
    {
        R_PTADR->RSELP[port] |= (uint8_t) (1U << pin_num);
        p_ioport_regs         = IOPORT_PRV_PORT_ADDRESS(IOPORT_REGION_SEL_NSAFE);
    }
    else                               // Setting for Safety region
    {
        R_PTADR->RSELP[port] &= (uint8_t) (~(1U << pin_num));
        p_ioport_regs         = IOPORT_PRV_PORT_ADDRESS(IOPORT_REGION_SEL_SAFE);
    }

    /* Setting DRCTL register */
    if (3U >= pin_num)
    {
        temp_value = p_ioport_regs->DRCTL[port].L & ~(IOPORT_DRTCL_BIT_MASK << (pin_num * 8U));
        p_ioport_regs->DRCTL[port].L = temp_value | (uint32_t) (p_cfg_data->drct_reg << (pin_num * 8U));
    }
    else if (3U < pin_num)
    {
        temp_value = p_ioport_regs->DRCTL[port].H & ~(IOPORT_DRTCL_BIT_MASK << ((pin_num - 4U) * 8U));
        p_ioport_regs->DRCTL[port].H = temp_value | (uint32_t) (p_cfg_data->drct_reg << ((pin_num - 4U) * 8U));
    }
    else
    {
        /* Do Nothing */
    }

    /* Setting for GPIO or peripheral */
    if (1U == p_cfg_data->pmc_reg)                                                                  // Setting for peripheral
    {
        temp_value               = p_ioport_regs->PFC[port] & ~(IOPORT_PFC_BIT_MASK << (pin_num * 4U));
        p_ioport_regs->PFC[port] = temp_value | (uint32_t) (p_cfg_data->pfc_reg << (pin_num * 4U)); // Setting PFC register

        /* Setting peripheral for port mode */
        p_ioport_regs->PMC[port] |= (uint8_t) (p_cfg_data->pmc_reg << pin_num);                     // Setting PMC register
    }
    else // Setting for GPIO
    {
        /* Setting GPIO for port mode */
        p_ioport_regs->PMC[port] &= (uint8_t) (~(1U << pin_num)); // Setting PMC register

        /* Setting for input or output */
        if (1U == p_cfg_data->pm_reg)                             // Setting for input
        {
            /* Setting PM register. */
            /* 01b: Input           */
            temp_value              = (uint32_t) (p_ioport_regs->PM[port] & ~(IOPORT_PM_BIT_MASK << (pin_num * 2U)));
            p_ioport_regs->PM[port] = (uint16_t) (temp_value | (uint32_t) (1U << (pin_num * 2U)));
        }
        else if (1U < p_cfg_data->pm_reg)     // Setting for two kinds of Output
        {
            /* Setting P register */
            if (0U == p_cfg_data->p_reg)      // Low output setting
            {
                p_ioport_regs->P[port] &= (uint8_t) (~(1U << pin_num));
            }
            else if (1U == p_cfg_data->p_reg) // High output setting
            {
                p_ioport_regs->P[port] |= (uint8_t) (1U << pin_num);
            }
            else
            {
                /* Do Nothing */
            }

            /* Setting PM register.                              */
            /* 10b: Output                                       */
            /* 11b: Output(output data is input to input buffer) */
            temp_value              = (uint32_t) (p_ioport_regs->PM[port] & ~(IOPORT_PM_BIT_MASK << (pin_num * 2U)));
            p_ioport_regs->PM[port] = (uint16_t) (temp_value | (uint32_t) (p_cfg_data->pm_reg << (pin_num * 2U)));
        }
        else
        {
            /* Do Nothing */
        }
    }
}






void pins_config (const ioport_cfg_t * p_cfg)
{
    uint16_t       pin_count;
    ioport_cfg_t * p_pin_data;

    p_pin_data = (ioport_cfg_t *) p_cfg;

    enable_pin_access();           // Unlock Register Write Protection

    for (pin_count = 0U; pin_count < p_pin_data->number_of_pins; pin_count++)
    {
        pin_set(p_pin_data->p_pin_cfg_data[pin_count].pin,
                         (ioport_cfg_data_t *) &p_pin_data->p_pin_cfg_data[pin_count].pin_cfg);
    }

    disable_pin_access();          // Lock Register Write Protection
}


void ioport_open (ioport_ctrl_t * const p_ctrl, const ioport_cfg_t * p_cfg)
{
    ioport_instance_ctrl_t * p_instance_ctrl = (ioport_instance_ctrl_t *) p_ctrl;

    /* Set driver status to open */
    p_instance_ctrl->open = IOPORT_OPEN;

    p_instance_ctrl->p_cfg = p_cfg;

    pins_config(p_cfg);

    ioport_event_config(p_cfg->p_extend);

}