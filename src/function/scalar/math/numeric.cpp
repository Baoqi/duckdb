#include "duckdb/function/scalar/math_functions.hpp"
#include "duckdb/common/vector_operations/vector_operations.hpp"

#include <algorithm>
#include <cmath>

using namespace duckdb;
using namespace std;

namespace duckdb {

//===--------------------------------------------------------------------===//
// abs
//===--------------------------------------------------------------------===//
struct AbsOperator {
	template <class T> static inline T Operation(T left) {
		return left < 0 ? left * -1 : left;
	}
};

void AbsFun::RegisterFunction(BuiltinFunctions &set) {
	ScalarFunctionSet abs("abs");
	for (auto &type : SQLType::NUMERIC) {
		abs.AddFunction(ScalarFunction({type}, type, ScalarFunction::GetScalarUnaryFunction<AbsOperator>(type)));
	}
	set.AddFunction(abs);
}

//===--------------------------------------------------------------------===//
// sign
//===--------------------------------------------------------------------===//
struct SignOperator {
	template <class T> static inline int8_t Operation(T left) {
		if (left == T(0))
			return 0;
		else if (left > T(0))
			return 1;
		else
			return -1;
	}
};

void SignFun::RegisterFunction(BuiltinFunctions &set) {
	ScalarFunctionSet sign("sign");
	for (auto &type : SQLType::NUMERIC) {
		sign.AddFunction(
		    ScalarFunction({type}, SQLType::TINYINT, ScalarFunction::GetScalarUnaryFunctionFixedReturn<int8_t, SignOperator>(type)));
	}
	set.AddFunction(sign);
}

//===--------------------------------------------------------------------===//
// ceil
//===--------------------------------------------------------------------===//
struct CeilOperator {
	template <class T> static inline T Operation(T left) {
		return ceil(left);
	}
};

void CeilFun::RegisterFunction(BuiltinFunctions &set) {
	ScalarFunctionSet ceil("ceil");
	for (auto &type : SQLType::NUMERIC) {
		scalar_function_t func;
		if (type.IsIntegral()) {
			// ceil on integral type is a nop
			func = ScalarFunction::NopFunction;
		} else {
			func = ScalarFunction::GetScalarUnaryFunction<CeilOperator>(type);
		}
		ceil.AddFunction(ScalarFunction({type}, type, func));
	}
	set.AddFunction(ceil);
	ceil.name = "ceiling";
	set.AddFunction(ceil);
}

//===--------------------------------------------------------------------===//
// floor
//===--------------------------------------------------------------------===//
struct FloorOperator {
	template <class T> static inline T Operation(T left) {
		return floor(left);
	}
};

void FloorFun::RegisterFunction(BuiltinFunctions &set) {
	ScalarFunctionSet floor("floor");
	for (auto &type : SQLType::NUMERIC) {
		scalar_function_t func;
		if (type.IsIntegral()) {
			// floor on integral type is a nop
			func = ScalarFunction::NopFunction;
		} else {
			func = ScalarFunction::GetScalarUnaryFunction<FloorOperator>(type);
		}
		floor.AddFunction(ScalarFunction({type}, type, func));
	}
	set.AddFunction(floor);
}

//===--------------------------------------------------------------------===//
// round
//===--------------------------------------------------------------------===//
struct RoundOperator {
	template <class T> static inline T Operation(T input, int32_t precision) {
		if (precision < 0) {
			precision = 0;
		}
		T modifier = pow(10, precision);
		return (round(input * modifier)) / modifier;
	}
};

void RoundFun::RegisterFunction(BuiltinFunctions &set) {
	ScalarFunctionSet round("round");
	for (auto &type : SQLType::NUMERIC) {
		scalar_function_t func;
		if (type.IsIntegral()) {
			// round on integral type is a nop
			func = ScalarFunction::NopFunction;
		} else {
			func = ScalarFunction::GetScalarBinaryFunctionFixedArgument<int8_t, RoundOperator>(type);
		}
		round.AddFunction(ScalarFunction({type, SQLType::INTEGER}, type, func));
	}
	set.AddFunction(round);
}

//===--------------------------------------------------------------------===//
// exp
//===--------------------------------------------------------------------===//
struct ExpOperator {
	template <class T> static inline double Operation(T left) {
		return exp(left);
	}
};

void ExpFun::RegisterFunction(BuiltinFunctions &set) {
	set.AddFunction(ScalarFunction("exp", {SQLType::DOUBLE}, SQLType::DOUBLE,
	                               ScalarFunction::UnaryFunction<double, double, ExpOperator>));
}

//===--------------------------------------------------------------------===//
// pow
//===--------------------------------------------------------------------===//
struct PowOperator {
	template <class T> static inline T Operation(T base, T exponent) {
		return pow(base, exponent);
	}
};

void PowFun::RegisterFunction(BuiltinFunctions &set) {
	ScalarFunction power_function("pow", {SQLType::DOUBLE, SQLType::DOUBLE}, SQLType::DOUBLE,
	                              ScalarFunction::BinaryFunction<double, double, double, PowOperator>);
	set.AddFunction(power_function);
	power_function.name = "power";
	set.AddFunction(power_function);
}

//===--------------------------------------------------------------------===//
// sqrt
//===--------------------------------------------------------------------===//
struct SqrtOperator {
	template <class T> static inline T Operation(T left) {
		return sqrt(left);
	}
};

void SqrtFun::RegisterFunction(BuiltinFunctions &set) {
	set.AddFunction(ScalarFunction("sqrt", {SQLType::DOUBLE}, SQLType::DOUBLE,
	                               ScalarFunction::UnaryFunction<double, double, SqrtOperator>));
}

//===--------------------------------------------------------------------===//
// ln
//===--------------------------------------------------------------------===//
struct LnOperator {
	template <class T> static inline T Operation(T left) {
		return log(left);
	}
};

void LnFun::RegisterFunction(BuiltinFunctions &set) {
	set.AddFunction(ScalarFunction("ln", {SQLType::DOUBLE}, SQLType::DOUBLE,
	                               ScalarFunction::UnaryFunction<double, double, LnOperator>));
}

//===--------------------------------------------------------------------===//
// log
//===--------------------------------------------------------------------===//
struct Log10Operator {
	template <class T> static inline T Operation(T left) {
		return log10(left);
	}
};

void Log10Fun::RegisterFunction(BuiltinFunctions &set) {
	ScalarFunction log_function("log10", {SQLType::DOUBLE}, SQLType::DOUBLE,
	                            ScalarFunction::UnaryFunction<double, double, Log10Operator>);
	set.AddFunction(log_function);
	// "log" is an alias for "log10"
	log_function.name = "log";
	set.AddFunction(log_function);
}

//===--------------------------------------------------------------------===//
// log2
//===--------------------------------------------------------------------===//
struct Log2Operator {
	template <class T> static inline T Operation(T left) {
		return log2(left);
	}
};

void Log2Fun::RegisterFunction(BuiltinFunctions &set) {
	set.AddFunction(ScalarFunction("log2", {SQLType::DOUBLE}, SQLType::DOUBLE,
	                               ScalarFunction::UnaryFunction<double, double, Log2Operator>));
}

//===--------------------------------------------------------------------===//
// cbrt
//===--------------------------------------------------------------------===//
struct CbRtOperator {
	template <class T> static inline double Operation(T left) {
		return cbrt(left);
	}
};

void CbrtFun::RegisterFunction(BuiltinFunctions &set) {
	set.AddFunction(ScalarFunction("cbrt", {SQLType::DOUBLE}, SQLType::DOUBLE,
	                               ScalarFunction::UnaryFunction<double, double, CbRtOperator>));
}

//===--------------------------------------------------------------------===//
// pi
//===--------------------------------------------------------------------===//
Value pi_value = Value::DOUBLE(PI);

static void pi_function(DataChunk &args, ExpressionState &state, Vector &result) {
	assert(args.column_count == 0);
	result.Reference(pi_value);
}

void PiFun::RegisterFunction(BuiltinFunctions &set) {
	set.AddFunction(ScalarFunction("pi", {}, SQLType::DOUBLE, pi_function));
}

//===--------------------------------------------------------------------===//
// degrees
//===--------------------------------------------------------------------===//
struct DegreesOperator {
	template <class T> static inline double Operation(T left) {
		return left * (180 / PI);
	}
};

void DegreesFun::RegisterFunction(BuiltinFunctions &set) {
	set.AddFunction(ScalarFunction("degrees", {SQLType::DOUBLE}, SQLType::DOUBLE,
	                               ScalarFunction::UnaryFunction<double, double, DegreesOperator>));
}

//===--------------------------------------------------------------------===//
// radians
//===--------------------------------------------------------------------===//
struct RadiansOperator {
	template <class T> static inline double Operation(T left) {
		return left * (PI / 180);
	}
};

void RadiansFun::RegisterFunction(BuiltinFunctions &set) {
	set.AddFunction(ScalarFunction("radians", {SQLType::DOUBLE}, SQLType::DOUBLE,
	                               ScalarFunction::UnaryFunction<double, double, RadiansOperator>));
}

} // namespace duckdb
