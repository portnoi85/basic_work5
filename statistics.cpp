#include <iostream>
#include <limits>
#include <vector>
#include <cmath>

class IStatistics {
public:
	virtual ~IStatistics() {}

	virtual void update(double next) = 0;
	virtual double eval() const = 0;
	virtual const char * name() const = 0;
};

class Min : public IStatistics {
public:
	Min() : m_min{std::numeric_limits<double>::max()} {
	}

	void update(double next) override {
		if (next < m_min) {
			m_min = next;
		}
	}

	double eval() const override {
		return m_min;
	}

	const char * name() const override {
		return "min";
	}

private:
	double m_min;
};

class Max : public IStatistics {
public:
	Max() : m_max{std::numeric_limits<double>::lowest()} {
	}

	void update(double next) override {
		if (next > m_max) {
			m_max = next;
		}
	}

	double eval() const override {
		return m_max;
	}

	const char * name() const override {
		return "max";
	}

private:
	double m_max;
};

class Mean : public IStatistics {
public:
	Mean() : m_count{0}, m_sum{0} {
	}

    void update(double next) override {
		m_sum += next;
		++m_count;
	}

    double eval() const override {
		if (m_count == 0) {
			return NAN;
		}
		return {m_sum/m_count};
	}

    const char* name() const override {
		return "mean";
	};

private:
    int m_count{0};
    double m_sum{0};
};

class Std : public IStatistics {
public:
	Std() = default;

    void update(double next) override {
		values.push_back(next);
	}

    double eval() const override {
		if (values.empty()) {
			return NAN;
		}
		Mean mean_;
		for (const double& val : values) {
			mean_.update(val);
		}
		double mean = mean_.eval();
		Mean disp;
		for (const double& val : values) {
			disp.update(std::pow(val - mean , 2));
		}
		return {std::sqrt(disp.eval())};
	}

    const char* name() const override {
		return "std";
	};

private:
	std::vector<double> values;
};

class Pct90 : public IStatistics {
public:
	Pct90() = default;

    void update(double next) override {
		auto pos = values.begin();
		int i = 0;
		while (pos != values.end() && *pos < next ) {
			++pos;
			++i;
		}
		values.insert(pos, next);
	}

    double eval() const override {
		if (values.empty()) {
			return NAN;
		}
		int size = values.size();
		int pos90 = floor(size * 90 / 100.0);
		return values[pos90];
	}

    const char* name() const override {
		return "pct90";
	};

private:
	std::vector<double> values;
};

class Pct95 : public IStatistics {
public:
	Pct95() = default;

    void update(double next) override {
		auto pos = values.begin();
		int i = 0;
		while (pos != values.end() && *pos < next ) {
			++pos;
			++i;
		}
		values.insert(pos, next);
	}

    double eval() const override {
		if (values.empty()) {
			return NAN;
		}
//		std::sort(values.begin(), values.end());
		int size = values.size();
		int pos95 = floor(size * 95 / 100.0);
		return values[pos95];
	}

    const char* name() const override {
		return "pct95";
	};

private:
	std::vector<double> values;
};

int main() {

	const size_t statistics_count = 6;
	IStatistics *statistics[statistics_count];

	statistics[0] = new Min{};
	statistics[1] = new Max{};
	statistics[2] = new Mean{};
	statistics[3] = new Std{};
	statistics[4] = new Pct90{};
	statistics[5] = new Pct95{};

	double val = 0;
	while (std::cin >> val) {
		for (size_t i = 0; i < statistics_count; ++i) {
			statistics[i]->update(val);
		}
	}

	// Handle invalid input data
	if (!std::cin.eof() && !std::cin.good()) {
		std::cerr << "Invalid input data\n";
		return 1;
	}


	// Print results if any
	for (size_t i = 0; i < statistics_count; ++i) {
		std::cout << statistics[i]->name() << " = " << statistics[i]->eval() << std::endl;
	}

	// Clear memory - delete all objects created by new
	for (size_t i = 0; i < statistics_count; ++i) {
		delete statistics[i];
	}

	return 0;
}