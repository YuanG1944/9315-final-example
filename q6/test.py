class LinearHashing:
    def __init__(self, initial_d=1, page_capacity=2, overflow_capacity=2):
        self.d = initial_d
        self.sp = 0
        self.page_capacity = page_capacity
        self.overflow_capacity = overflow_capacity
        self.pages = [{'main': [], 'overflow': []} for _ in range(2 ** initial_d)]
    
    def _hash(self, key, depth):
        """计算哈希值：取二进制后depth位"""
        binary = bin(key)[2:].zfill(8)  # 8位二进制，前面补零
        return int(binary[-depth:], 2) if depth != 0 else 0
    
    def _insert_into_bucket(self, bucket, key):
        """将键插入桶的主页或溢出页"""
        if len(bucket['main']) < self.page_capacity:
            bucket['main'].append(key)
        else:
            if not bucket['overflow'] or len(bucket['overflow'][-1]) >= self.overflow_capacity:
                bucket['overflow'].append([])
            bucket['overflow'][-1].append(key)
    
    def _split_bucket(self):
        """分裂sp指向的桶"""
        if self.sp >= len(self.pages):
            return
        
        # 收集原桶的所有键
        original = self.pages[self.sp]
        all_keys = original['main'].copy()
        for op in original['overflow']:
            all_keys.extend(op)
        original['main'].clear()
        original['overflow'].clear()
        
        # 创建新桶
        new_idx = len(self.pages)
        self.pages.append({'main': [], 'overflow': []})
        
        # 重新哈希所有键（使用d+1位）
        new_depth = self.d + 1
        for key in all_keys:
            h_new = self._hash(key, new_depth)
            target = self.sp if h_new == self.sp else new_idx
            self._insert_into_bucket(self.pages[target], key)
        
        # 更新sp和d
        self.sp += 1
        if self.sp >= (2 ** self.d):
            self.d += 1
            self.sp = 0
    
    def insert(self, key, split_before=False):
        """插入键，split_before为True表示插入前分裂"""
        if split_before:
            self._split_bucket()
        bucket_idx = self._hash(key, self.d)
        self._insert_into_bucket(self.pages[bucket_idx], key)
    
    def get_state(self):
        """返回当前状态的字符串表示"""
        state = []
        for idx, page in enumerate(self.pages):
            line = f"[{idx}] {','.join(map(str, page['main']))}"
            for op in page['overflow']:
                line += f" -> {','.join(map(str, op))}"
            state.append(line)
        return '\n'.join(state) + f'\nd = {self.d} sp = {self.sp}\n'

# 示例用法
split_points = {6, 12, 18, 24, 30}  # 插入这些键前分裂
lh = LinearHashing()

for key in range(1, 33):
    if key in split_points:
        # 输出分裂前的状态
        print(f"Before inserting {key} (Split triggered):")
        print(lh.get_state())
        # 分裂并插入键
        lh.insert(key, split_before=True)
        # 输出分裂后的状态
        print(f"After inserting {key}:")
        print(lh.get_state())
    else:
        lh.insert(key)