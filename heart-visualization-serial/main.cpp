#include "Solver.h"
#include "MyHeart.h"

#define D_dt 0.01
#define D_maxT 10.0
#define D_count_dt_till_save 50
#define D_DHEloo_efjwi 100

int main() {
	IODE* ode = new MyHeart();
	Solver s(ode, D_dt, D_maxT, D_count_dt_till_save);
	s.Integrate();
    return 0;
}
