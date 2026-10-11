// 담당: 장데니스
// 관측소 노드 그래프. 관측소 = 노드, 가까운 관측소끼리 간선
// Top/Worst 지역 순위, 바람 기반 미세먼지 추정에 사용합니다.
#pragma once
#include "common/Types.h"

struct RankItem {
    std::string name;
    double score = 0.0;
};

class StationGraph {
public:
    // 관측소마다 가까운 k개와 연결
    StationGraph(const DynamicArray<Station>& stations, int k);

    // 종합 환경 점수 Top k (좋은 순) / Worst k (나쁜 순)
    void topK(int k, DynamicArray<RankItem>& out) const;
    void worstK(int k, DynamicArray<RankItem>& out) const;

    // 풍향·풍속을 따라 이웃 관측소로 미세먼지가 이동한다고 보고 steps 단계 뒤 농도 추정
    void estimateByWind(int steps, DynamicArray<double>& outPm25) const;

private:
    const DynamicArray<Station>& stations_;
    // TODO(장데니스): 인접 리스트
};
