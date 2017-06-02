module FuncModule
    implicit none
    contains
        subroutine testExp(N, rep)
 
            real, dimension(:), allocatable :: dataArr
            integer :: N, rep, istat = 0
            real    :: sum, TotalQty
            real    :: t, t1, t2
            integer :: i, j

            allocate(dataArr(4*N), stat = istat)
            if (istat /= 0) stop 'Error during allocation'

            do i = 0, 4*N
                dataArr(i) = 1 + rand(0) * 10.0
            end do

            call cpu_time(t1)
            do j = 0, rep - 1
                do i = 0, 4*N
                    sum = sum + exp(dataArr(i)); 
                end do
            end do
            

            !loop unrolling doesn't lead to improved performance
            !do j = 0, rep - 1
            !    do i = 0, 4*(N-1), 4
            !        sum = sum + exp(dataArr(i  )); 
            !        sum = sum + exp(dataArr(i+1)); 
            !        sum = sum + exp(dataArr(i+2)); 
            !        sum = sum + exp(dataArr(i+3)); 
            !    end do
            !end do

            call cpu_time(t2)
            t = t2 - t1

            print*,"Ignore:", sum
            TotalQty = rep * N * 4
            print*,"Exps computed:", TotalQty, "time ",(t * 1e3), "ms"
            print*,"Milllions of Exps per sec:", (TotalQty / 1e6 / t)

            deallocate(dataArr, stat = istat)
            if (istat /= 0) stop 'Error during deallocation'

        end subroutine testExp

        subroutine testLog(N, rep)
 
            real, dimension(:), allocatable :: dataArr
            integer :: N, rep, istat = 0
            real    :: sum, TotalQty
            real    :: t, t1, t2
            integer :: i, j

            allocate(dataArr(4*N), stat = istat)
            if (istat /= 0) stop 'Error during allocation'

            do i = 0, 4*N
                dataArr(i) = 1 + rand(0) * 10.0
            end do

            call cpu_time(t1)
            do j = 0, rep - 1
                do i = 0, 4*N
                    sum = sum + log(dataArr(i)); 
                end do
            end do

            call cpu_time(t2)
            t = t2 - t1

            print*,"Ignore:", sum
            TotalQty = rep * N * 4
            print*,"Logs computed:", TotalQty, "time ",(t * 1e3), "ms"
            print*,"Milllions of Logs per sec:", (TotalQty / 1e6 / t)

            deallocate(dataArr, stat = istat)
            if (istat /= 0) stop 'Error during deallocation'

        end subroutine testLog
end module FuncModule

program main
    use FuncModule

    call testExp(10000, 2000)
    call testLog(10000, 2000)

end program main
