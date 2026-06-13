#pragma once
#include "mixins.hpp"

namespace usage {
class Customer
    : public mixins::UniqueId<std::string>
    , public mixins::TypeId<Customer>
    , public mixins::CountInstances<Customer> {
public:
  explicit Customer(std::string id) : id(std::move(id)), purchases(0), moneySpent(0) {}

  const std::string &getId() const noexcept override { return id; }

  [[nodiscard]] size_t getPurchases() const noexcept { return purchases; }
  [[nodiscard]] double getMoneySpent() const noexcept { return moneySpent; }

  void addPurchase(double amount) noexcept {
    purchases += 1;
    moneySpent += amount;
  }

private:
  std::string id;
  size_t purchases;
  double moneySpent;
};

class Summary
    : public mixins::BitRepr<Summary>
    , public mixins::TypeId<Summary>
    , public mixins::CountInstances<Summary> {
public:
  Summary() : uniqueCustomers(0), totalPurchases(0), revenue(0) {}
  Summary(size_t uniqueCustomers, size_t totalPurchases, double revenue)
      : uniqueCustomers(uniqueCustomers), totalPurchases(totalPurchases), revenue(revenue) {}

  [[nodiscard]] size_t getUniqueCustomers() const noexcept { return uniqueCustomers; }
  [[nodiscard]] size_t getTotalPurchases() const noexcept { return totalPurchases; }
  [[nodiscard]] double getRevenue() const noexcept { return revenue; }

  bool operator==(const Summary &other) const {
    return uniqueCustomers == other.uniqueCustomers && totalPurchases == other.totalPurchases &&
           revenue == other.revenue;
  }

private:
  size_t uniqueCustomers;
  size_t totalPurchases;
  double revenue;
};

class Item
    : public mixins::UniqueId<size_t>
    , public mixins::BitRepr<Item>
    , public mixins::TypeId<Item>
    , public mixins::CountInstances<Item> {
public:
  Item() : id(), quantity(), value() {}
  Item(size_t id, size_t quantity, double value) : id(id), quantity(quantity), value(value) {}

  [[nodiscard]] const size_t &getId() const noexcept override { return id; }
  [[nodiscard]] size_t getQuantity() const noexcept { return quantity; }
  [[nodiscard]] double getValue() const noexcept { return value; }

private:
  size_t id;
  size_t quantity;
  double value;
};
} // namespace usage