/**
 * @file:   usr_proc.c
 * @brief:  Two user processes: proc1 and proc2
 * @author: Yiqing Huang
 * @date:   2014/01/17
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */

#include "rtx.h"
#include "uart_polling.h"
#include "usr_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

int num_tests = 0;
int num_tests_passed = 0;
int num_tests_failed = 0;

/* initialization table item */
PROC_INIT g_test_procs[NUM_TEST_PROCS];

void set_test_procs() {
	int i;
	for( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_test_procs[i].m_pid=(U32)(i+1);
		g_test_procs[i].m_priority=LOWEST;
		g_test_procs[i].m_stack_size=0x100;
	}
  
	g_test_procs[0].mpf_start_pc = &proc1;
	g_test_procs[1].mpf_start_pc = &proc2;
}


/**
 * @brief: a process that prints five uppercase letters
 *         and then yields the cpu.
 */
void proc1(void)
{
	int i;
#ifdef DEBUG_0
	printf("G019_test: START");
#endif
	
	for ( i = 1; i < NUM_TEST_PROCS; i++) {
		set_process_priority(i + 1, LOW);
	}
	
	release_processor();
	
#ifdef DEBUG_0
	printf("G019_test: %d/%d tests OK", num_tests_passed, num_tests);
	printf("G019_test: %d/%d tests FAIL", num_tests_failed, num_tests);
	printf("G019_test: END");
#endif
	
	while (true) {};
}
	
void proc2(void)
{
	int prio_6 = get_process_priority(6);
	
	set_process_priority(6, HIGH);
	num_tests++;
	if (prio_6 == HIGH) {
#ifdef DEBUG_0
		printf("G019_test: test %d OK", num_tests);
#endif
		num_tests_passed++;
	} else {
#ifdef DEBUG_0
		printf("G019_test: test %d FAIL", num_tests);
#endif
		num_tests_failed++;
	}
	release_processor();
}

void proc6(void)
{
	int prev_prio = LOW;
	
	num_tests++;
	
	int prio = get_process_priority(6);
	
	if (prio != HIGH) {
#ifdef DEBUG_0
		printf("G019_test: test %d FAIL", num_tests);
#endif
		num_tests_failed++;
		set_process_priority(6, LOWEST);
	}
	
	set_process_priority(6, MEDIUM);
	release_processor();
	
	if (prio != MEDIUM) {
#ifdef DEBUG_0
		printf("G019_test: test %d FAIL", num_tests);
#endif
		num_tests_failed++;
		set_process_priority(6, LOWEST);
	}
	
	set_process_priority(6, LOW);
	release_processor();
	
	if (prio != LOW) {
#ifdef DEBUG_0
		printf("G019_test: test %d FAIL", num_tests);
#endif
		num_tests_failed++;
		set_process_priority(6, LOWEST);
	}
	
#ifdef DEBUG_0
	printf("G019_test: test %d OK", num_tests);
#endif
	num_tests_passed++;
	set_process_priority(6, LOWEST);
	release_processor();
}

/**
 * @brief: a process that prints five numbers
 *         and then yields the cpu.
 */
void proc2(void)
{
	int i = 0;
	int ret_val = 20;
	while ( 1) {
		if ( i != 0 && i%5 == 0 ) {
			uart0_put_string("\n\r");
			ret_val = release_processor();
#ifdef DEBUG_0
			printf("proc2: ret_val=%d\n", ret_val);
#endif /* DEBUG_0 */
		}
		uart0_put_char('0' + i%10);
		i++;
	}
}

/**
 * @brief
 * 4, 5. set partner process to current level - 1, partner does same,
 * until they reach max, then decrement self till lowest, then repeat
 */

void expect_4_5(int a, int b) {
  if (get_process_priority(4) != a ||
      get_process_priority(5) != b) {
    num_tests_failed++;
    set_process_priority(4, LOWEST);
    set_process_priority(5, LOWEST);
    release_processor();
  }

  // should never be reached
  while (1) {};
}

void proc4(void) {
  num_tests++;

  expect_4_5(LOW, LOW);

  set_process_priority(5, HIGH);

  release_processor();

  expect_4_5(HIGH, HIGH);

  set_process_priority(5, MEDIUM);

  release_processor();

  expect_4_5(MEDIUM, MEDIUM);

  set_process_priority(5, LOW);

  release_processor();

  expect_4_5(LOW, LOW);

  set_process_priority(4, LOWEST);
  set_process_priority(5, LOWEST);

  num_tests_passed++;

  release_processor();
}

void proc5(void) {
  expect_4_5(LOW, HIGH);

  set_process_priority(4, HIGH);

  release_processor();

  expect_4_5(MEDIUM, HIGH);

  set_process_priority(4, MEDIUM);

  release_processor();

  expect_4_5(LOW, MEDIUM);

  set_process_priority(4, LOW);

  release_processor();
 }

void proc3(void) {
	int i;
	int *first = request_memory_block();
	int *elem = first;
	
	for (i = 0; i < 10; i++){
		*elem = i;
		elem++;
	}
	
	num_tests++;
	
	if (RTX_OK == release_memory_block(first)){
#ifdef DEBUG 0
		printf("G019_test: test %d OK", num_tests);
#endif
		num_tests_passed++;
	}
	else {
#ifdef DEBUG 0
		printf("G019_test: test %d FAIL", num_tests);
#endif
		num_test_failed++;
	}
	
	set_process_priority(3, LOWEST);
	release_processor();
}
