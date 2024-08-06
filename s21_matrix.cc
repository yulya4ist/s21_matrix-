#include "s21_matrix.h"

S21Matrix::S21Matrix() : rows_(3), cols_(3) { CreateMatrix(rows_, cols_); }

S21Matrix::S21Matrix(int rows, int cols) {
  if (rows > 0 && cols > 0) {
    rows_ = rows;
    cols_ = cols;
    CreateMatrix(rows, cols);
  } else {
    throw std::out_of_range("Invalid matrix");
  }
}

S21Matrix::S21Matrix(const S21Matrix& other)
    : rows_(other.rows_), cols_(other.cols_) {
  matrix_ = new double*[other.rows_];
  for (int i = 0; i < other.rows_; i++) {
    matrix_[i] = new double[other.cols_];
  }
  for (int i = 0; i < other.rows_; i++) {
    for (int j = 0; j < other.cols_; j++) {
      matrix_[i][j] = other.matrix_[i][j];
    }
  }
}

S21Matrix::S21Matrix(S21Matrix&& other) noexcept
    : matrix_(other.matrix_), rows_(other.rows_), cols_(other.cols_) {
  other.cols_ = 0;
  other.rows_ = 0;
  other.matrix_ = nullptr;
}

S21Matrix::~S21Matrix() {
  for (int i = 0; i < rows_; i++) {
    delete[] matrix_[i];
  }
  delete[] matrix_;
  rows_ = 0;
  cols_ = 0;
  matrix_ = nullptr;
}

void S21Matrix::CreateMatrix(int rows, int columns) {
  matrix_ = new double*[rows];
  for (int i = 0; i < rows; i++) {
    matrix_[i] = new double[columns]();
  }
}

bool S21Matrix::_CheckMatrix(const S21Matrix& other) const noexcept {
  bool res = NO_PROBLEMO;
  if (matrix_ == nullptr || other.matrix_ == nullptr) res = FAILURE;
  if (rows_ < 1 || cols_ < 1 || other.rows_ < 1 || other.cols_ < 1)
    res = FAILURE;
  return res;
}

void S21Matrix::_FillMatrix(double val) noexcept {
  for (int i = 0; i < rows_; i++) {
    for (int j = 0; j < cols_; j++) {
      matrix_[i][j] = i + j + val;
    }
  }
}

bool S21Matrix::EqMatrix(const S21Matrix& other) const noexcept {
  int res = NO_PROBLEMO;
  res = _CheckMatrix(other);
  if (res != FAILURE && rows_ == other.rows_ && cols_ == other.cols_) {
    for (int i = 0; i < rows_ && res != FAILURE; i++) {
      for (int j = 0; j < cols_ && res != FAILURE; j++) {
        if (fabs(matrix_[i][j] - other.matrix_[i][j]) > 1e-7) res = FAILURE;
      }
    }
  } else {
    res = FAILURE;
  }
  return res;
}

void S21Matrix::SumMatrix(const S21Matrix& other) {
  if (rows_ != other.rows_ || cols_ != other.cols_) {
    throw std::invalid_argument("Sizes are not equal");
  }
  char plus = '+';
  _SumAndSubMatrix(plus, other);
}

void S21Matrix::SubMatrix(const S21Matrix& other) {
  if (rows_ != other.rows_ || cols_ != other.cols_) {
    throw std::invalid_argument("Sizes are not equal");
  }
  char minus = '-';
  _SumAndSubMatrix(minus, other);
}

void S21Matrix::_SumAndSubMatrix(char plus_or_minus, const S21Matrix& other) {
  bool res = _CheckMatrix(other);
  if (res) {
    S21Matrix result(rows_, cols_);
    for (int i = 0; i < rows_; i++) {
      for (int j = 0; j < cols_; j++) {
        if (plus_or_minus == '-')
          result.matrix_[i][j] = matrix_[i][j] - other.matrix_[i][j];
        else {
          result.matrix_[i][j] = matrix_[i][j] + other.matrix_[i][j];
        }
      }
    }
    *this = result;
  } else {
    throw std::out_of_range("Invalid matrix");
  }
}

void S21Matrix::MulNumber(const double num) {
  if (matrix_ != nullptr || cols_ > 0 || rows_ > 0) {
    S21Matrix result(rows_, cols_);
    for (int i = 0; i < rows_; i++) {
      for (int j = 0; j < cols_; j++) {
        result.matrix_[i][j] = matrix_[i][j] * num;
      }
    }
    *this = result;
  } else {
    throw std::out_of_range("Invalid matrix");
  }
}

void S21Matrix::MulMatrix(const S21Matrix& other) {
  bool res = _CheckMatrix(other);
  if (cols_ != other.rows_)
    throw std::invalid_argument(
        "Columns first matrix not equal rows second matrix");
  else if (res) {
    S21Matrix result(rows_, other.cols_);
    for (int i = 0; i < rows_; i++) {
      for (int j = 0; j < other.cols_; j++) {
        for (int n = 0; n < other.rows_; n++) {
          result.matrix_[i][j] += matrix_[i][n] * other.matrix_[n][j];
        }
      }
    }
    *this = result;
  } else {
    throw std::out_of_range("Invalid matrix");
  }
}

S21Matrix S21Matrix::Transpose() {
  if (matrix_ == nullptr || cols_ < 1 || rows_ < 1) {
    throw std::out_of_range("Invalid matrix");
  }
  S21Matrix result(cols_, rows_);
  for (int i = 0; i < rows_; i++) {
    for (int j = 0; j < cols_; j++) {
      result.matrix_[j][i] = matrix_[i][j];
    }
  }
  return result;
}

double S21Matrix::Determinant() {
  double det = 0.;
  if (rows_ != cols_)
    throw std::invalid_argument("Matrix is not square");
  else if (matrix_ == nullptr || cols_ < 1 || rows_ < 1)
    throw std::out_of_range("Invalid matrix");
  det = _Matrix_Determinant(*this, rows_, cols_);
  return det;
}

double S21Matrix::_Matrix_Determinant(const S21Matrix& other, int row,
                                      int column) {
  double det = 0;
  if (column == 2 && row == 2) {
    det += other.matrix_[0][0] * other.matrix_[1][1] -
           other.matrix_[0][1] * other.matrix_[1][0];
  } else if (column == 1 && row == 1) {
    det += other.matrix_[0][0];
  } else if (column > 2 && row > 2) {
    S21Matrix new_matrix(row, column);
    for (int i = 0; i < column; i++) {
      int matrix_i = 0;
      for (int j = 1; j < row; j++) {
        int matrix_j = 0;
        for (int k = 0; k < row; k++) {
          if (k != i) {
            new_matrix.matrix_[matrix_i][matrix_j] = other.matrix_[j][k];
            matrix_j++;
          }
        }
        matrix_i++;
      }
      det += other.matrix_[0][i] * pow(-1, i) *
             _Matrix_Determinant(new_matrix, row - 1, column - 1);
    }
  }
  return det;
}

S21Matrix S21Matrix::CalcComplements() {
  if (rows_ != cols_)
    throw std::invalid_argument("Matrix is not square");
  else if (matrix_ == nullptr || cols_ < 1 || rows_ < 1)
    throw std::out_of_range("Invalid matrix");
  S21Matrix new_matrix(rows_, cols_);
  S21Matrix result(rows_, cols_);
  for (int i = 0; i < rows_; i++) {
    for (int j = 0; j < cols_; j++) {
      int matrix_i = 0;
      for (int x = 0; x < cols_; x++) {
        if (x == i) {
          continue;
        }
        int matrix_j = 0;
        for (int y = 0; y < rows_; y++) {
          if (y != j) {
            new_matrix.matrix_[matrix_i][matrix_j] = matrix_[x][y];
            matrix_j++;
          }
        }
        matrix_i++;
      }
      result.matrix_[i][j] =
          _Matrix_Determinant(new_matrix, rows_ - 1, cols_ - 1) *
          pow(-1, i + j);
    }
  }
  return result;
}

S21Matrix S21Matrix::InverseMatrix() {
  if (matrix_ == nullptr || cols_ < 1 || rows_ < 1)
    throw std::out_of_range("Invalid matrix");
  double det = 0;
  det = Determinant();
  if (det == 0) throw std::invalid_argument("Determinant equals 0");

  S21Matrix temp(rows_, cols_);
  temp = CalcComplements();
  temp = temp.Transpose();
  for (int i = 0; i < cols_; i++) {
    for (int j = 0; j < rows_; j++) {
      temp.matrix_[i][j] /= det;
    }
  }
  return temp;
}

int S21Matrix::GetRows() const noexcept { return rows_; }
int S21Matrix::GetCols() const noexcept { return cols_; }

void S21Matrix::SetRows(int rows) {
  S21Matrix result(rows, cols_);
  if (rows > rows_) {
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols_; j++) {
        if (i >= rows_)
          result.matrix_[i][j] = 0;
        else
          result.matrix_[i][j] = matrix_[i][j];
      }
    }
  }
  *this = result;
}
void S21Matrix::SetCols(int cols) {
  S21Matrix result(rows_, cols);
  if (cols > cols_) {
    for (int i = 0; i < rows_; i++) {
      for (int j = 0; j < cols; j++) {
        if (j >= cols_)
          result.matrix_[i][j] = 0;
        else
          result.matrix_[i][j] = matrix_[i][j];
      }
    }
  }
  *this = result;
}

S21Matrix& S21Matrix::operator=(const S21Matrix& other) {
  for (int i = 0; i < rows_; i++) {
    delete[] matrix_[i];
  }
  delete[] matrix_;
  cols_ = other.cols_;
  rows_ = other.rows_;
  matrix_ = new double*[other.rows_];
  for (int i = 0; i < other.rows_; i++) {
    matrix_[i] = new double[other.cols_];
  }
  for (int i = 0; i < other.rows_; i++) {
    for (int j = 0; j < other.cols_; j++) {
      matrix_[i][j] = other.matrix_[i][j];
    }
  }
  return *this;
}

S21Matrix& S21Matrix::operator=(S21Matrix&& other) noexcept {
  for (int i = 0; i < rows_; i++) {
    delete[] matrix_[i];
  }
  delete[] matrix_;
  cols_ = other.cols_;
  rows_ = other.rows_;
  matrix_ = other.matrix_;
  other.cols_ = 0;
  other.rows_ = 0;
  other.matrix_ = nullptr;
  return *this;
}

double& S21Matrix::operator()(int i, int j) {
  if (i >= rows_ || j >= cols_ || i < 0 || j < 0) {
    throw std::out_of_range("Invalid index");
  }
  return matrix_[i][j];
}

S21Matrix S21Matrix::operator+(const S21Matrix& other) const {
  S21Matrix result(*this);
  result.SumMatrix(other);
  return result;
}

S21Matrix& S21Matrix::operator+=(const S21Matrix& other) {
  SumMatrix(other);
  return *this;
}

S21Matrix S21Matrix::operator-(const S21Matrix& other) const {
  S21Matrix result(*this);
  result.SubMatrix(other);
  return result;
}

S21Matrix& S21Matrix::operator-=(const S21Matrix& other) {
  SubMatrix(other);
  return *this;
}

S21Matrix S21Matrix::operator*(const S21Matrix& other) const {
  S21Matrix result(*this);
  result.MulMatrix(other);
  return result;
}

S21Matrix& S21Matrix::operator*=(const S21Matrix& other) {
  MulMatrix(other);
  return *this;
}

S21Matrix S21Matrix::operator*(const double num) const {
  S21Matrix result(*this);
  result.MulNumber(num);
  return result;
}

S21Matrix operator*(const double num, const S21Matrix& other) {
  S21Matrix result(other);
  result.MulNumber(num);
  return result;
}

S21Matrix& S21Matrix::operator*=(const double num) {
  MulNumber(num);
  return *this;
}

bool S21Matrix::operator==(const S21Matrix& other) const noexcept {
  return EqMatrix(other);
}