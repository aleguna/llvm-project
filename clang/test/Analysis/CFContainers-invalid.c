// RUN: %clang_cc1 -analyze -analyzer-checker=osx.coreFoundation.containers.PointerSizedValues -triple x86_64-apple-darwin -verify %s

typedef const struct __CFAllocator * CFAllocatorRef;
typedef const struct __CFArray * CFArrayRef;
typedef const struct __CFDictionary * CFDictionaryRef;
typedef const struct __CFSet * CFSetRef;

extern const CFAllocatorRef kCFAllocatorDefault;

// Unexpected declarations for these:
CFArrayRef CFArrayCreate(CFAllocatorRef);
CFDictionaryRef CFDictionaryCreate(CFAllocatorRef);
CFSetRef CFSetCreate(CFAllocatorRef);

void testNoCrash() {
  (void)CFArrayCreate(kCFAllocatorDefault);
  (void)CFDictionaryCreate(kCFAllocatorDefault);
  (void)CFSetCreate(kCFAllocatorDefault);
}
