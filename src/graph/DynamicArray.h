// 담당: 김재겸
// 동적 배열. std::vector 대신 모든 모듈에서 사용합니다. (가장 먼저 완성)
// 템플릿이므로 구현도 이 파일 안에 작성합니다.
#pragma once

template <typename T>
class DynamicArray {
public:
    DynamicArray();
    explicit DynamicArray(int initialCapacity);
    DynamicArray(const DynamicArray& other);             // 깊은 복사
    DynamicArray& operator=(const DynamicArray& other);  // 깊은 복사
    ~DynamicArray();

    void push_back(const T& value);   // 끝에 추가, 꽉 차면 용량 2배 확장 (상각 O(1))
    void pop_back();                  // 끝 원소 제거 O(1)
    T& operator[](int index);         // O(1)
    const T& operator[](int index) const;
    int size() const;
    bool empty() const;
    void clear();
    void resize(int newSize, const T& fill = T());  // 크기를 바꾸고 새 칸은 fill로 채움

private:
    T* data_ = nullptr;
    int size_ = 0;
    int capacity_ = 0;
};

// TODO(김재겸): 아래에 구현 작성
