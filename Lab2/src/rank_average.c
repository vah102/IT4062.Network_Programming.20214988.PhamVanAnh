#include <stdio.h>

char get_grade(double mean){
    if (mean >= 90) return 'S';
    if (mean >= 80) return 'A';	
    if (mean >= 70) return 'B';	
    if (mean >= 60) return 'C';
    return 'D';
}

int main(){
    static struct student data[]={
        ("Tuan", 82, 72, 58, 0.0),
        ("Nam", 77, 82, 79, 0.0),
        ("Khanh", 52, 62, 39, 0.0),
        ("Phuong", 61, 82, 88, 0.0) 
    };  

    int num_students = sizeof(data) / sizeof(data[0]);
    for (int i = 0; i < num_students; i++){
        data[i].mean = (data[i].eng + data[i].math + data[i].phys)/num_students;
        data[i].grade = get_grade(data[i].mean);
    }

    printf("Điểm trung bình của sinh viên và xếp hạng: \n");
    for (int i = 0; i < num_students; i++){
        printf("%s: %.2f, Grade: %c\n", data[i].name,
                                        data[i].mean,
                                        data[i].grade);
    }
    return 0;

}
