#ifndef REPLACEMENT_STRATEGY
#define REPLACEMENT_STRATEGY

class ReplacementStrategy {
 public:
  virtual void replace() = 0;
};

class LRUStrategy: ReplacementStrategy {
 public:
  void replace() override {
    
  }
};

#endif